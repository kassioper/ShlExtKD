#include "StdAfx.h"
#include <algorithm>
#include "windows.h"
#include "Shlobj.h"
#include "FilesInf.h"
#include "FilesInfViewDlg.h"
#include "FileInfThreads.h"
#include "FilesUtils.h"
#include "FileInfFormatter.h"


CFilesInf::~CFilesInf(void) 
{ 
	Clear();
}

void CFilesInf::Init()
{
	//
}


void CFilesInf::Clear()
{
	while(!m_FilesInf.empty())
	{
		delete m_FilesInf.back();
		m_FilesInf.pop_back();
	}
	//m_FilesInf.clear();
}

void CFilesInf::FileAdd(tstring *path)
{
	std::transform(path->begin(), path->end(), path->begin(), ::tolower);
	CFileInf *fileInf = new CFileInf(path);
	m_FilesInf.push_back(fileInf);
}

void CFilesInf::FileAdd(TCHAR *path)
{
	tstring *stPath = new tstring(path);
	FileAdd(stPath);
}

bool Compare(const CFileInf* l, const CFileInf* r) { 
	return FileNameCompare(l->Path, r->Path);
}


BOOL CFilesInf::FilesDetailsRead(HWND hWnd)
{
//	isAbort = false;

	m_evAbort = CreateEvent(NULL, TRUE, FALSE, NULL); // ManualReset, NotSet - init
	m_evItemComplete = CreateEvent(NULL, FALSE, FALSE, NULL); // AutoReset, NotSet - init
	m_evFinish = CreateEvent(NULL, TRUE, FALSE, NULL); // ManualReset, NotSet - init

	m_ThreadCount = 0;

	IMalloc *pIMalloc;
	HRESULT hr = SHGetMalloc(&pIMalloc);
	if (FAILED(hr))
		return 0;
	::QueryPerformanceCounter(&qpcStart);


	if (threadsMaxCount)
	{
		const int threadsCount = 3;
		_sntprintf_s((TCHAR*)szDescr, ARRAYSIZE(szDescr), _TRUNCATE, _T("Lim=%d"), threadsCount);
		m_ThreadItemsCount = 0;
		for (int i = threadsCount; i > 0; --i)
		{
			FileDetailsLaunchThreadArgs* pArgs = 
			(FileDetailsLaunchThreadArgs*) pIMalloc->Alloc
				(sizeof(FileDetailsLaunchThreadArgs));

			pArgs->filesInf = this;
			pArgs->hWnd = hWnd;

  		InterlockedIncrement(&m_ThreadCount);

			QueueUserWorkItem( FilesInfThreadFuncLim, (PVOID) pArgs, WT_EXECUTEDEFAULT );
		}
	}
	else
	{
		_tcscpy(szDescr, _T("Unlim"));
		FileDetailsLaunchThreadArgs* pArgs = 
		(FileDetailsLaunchThreadArgs*) pIMalloc->Alloc
			(sizeof(FileDetailsLaunchThreadArgs));

		pArgs->filesInf = this;
		pArgs->hWnd = hWnd;
		
		InterlockedIncrement(&m_ThreadCount);

		QueueUserWorkItem( FilesInfThreadFuncUnlim, (PVOID) pArgs, WT_EXECUTEDEFAULT );
		//OutputDebugString(_T("FilesInfThreadFuncLog After Call\n"));
	}

	Sleep(1); //? win XP bug
	m_LogOutFirstItem = 0;
	QueueUserWorkItem( FilesInfThreadFuncLog, (PVOID) this, WT_EXECUTEDEFAULT );

	pIMalloc->Release();
	return TRUE; 
}

void CFilesInf::FilesDetailsReadStop()
{
	//isAbort = true;
	if (NULL == m_evAbort)
		return;
	SetEvent(m_evAbort);

	DWORD dwRes = WaitForSingleObject(m_evFinish, 10 * 1000);
	if (WAIT_OBJECT_0 != dwRes)
	{
		if (IsDebuggerPresent())
			DebugBreak();   // WAIT_TIMEOUT = 258
	}
	//Sleep(0);
	//for (int waitCount = 10; 0 < waitCount; --waitCount)
	//{
	//	if (0 == m_ThreadCount)
	//		break;
	//	Sleep(50);
	//}

	CloseHandle(m_evAbort);
	m_evAbort = NULL; //?
	CloseHandle(m_evItemComplete);
	m_evItemComplete = NULL; //?
	CloseHandle(m_evFinish);
	m_evFinish = NULL; //?
}

void CFilesInf::OnThreadEnd()
{
	//OutputDebugString(_T("OnThreadEnd\n"));
	if (0 == InterlockedDecrement(&m_ThreadCount))
		SetEvent(m_evFinish);
}

INT_PTR CFilesInf::FilesInfViewDlgShow(HINSTANCE hInstance, HWND hWnd)
{
	std::sort(m_FilesInf.begin(), m_FilesInf.end(), Compare);

	return ::FileInfViewDlgShow(hInstance, hWnd, this);
}

void CFilesInf::FileDetailsRead(size_t itemID)
{
	CFileInf &fileInf = *m_FilesInf[itemID];
	pFnFileDetailsRead pfnFileDetailsRead;
	if (mode & FilesInfMode_FILE_MAPPING)
		pfnFileDetailsRead = &FileDetailsReadMap;
	else
		pfnFileDetailsRead = &FileDetailsReadStd;

	DWORD rez = (*pfnFileDetailsRead)(
			fileInf.Path->c_str()
			, fileInf.Size
			, fileInf.TimeCreate
			, fileInf.CheckSum
			, m_evAbort //&isAbort
		);

	if (NO_ERROR != rez)
	{
		fileInf.CheckSum = 1; // Size = 0 - error 
	}

	//return rez;
}

#ifdef _DEBUG
int CFilesInf::Debug(int n)
{
	return 0;
}
#endif

void CFilesInf::PerformanceShow(HWND hWnd)
{
	LARGE_INTEGER freq, qpcEnd;
	::QueryPerformanceCounter(&qpcEnd);
	qpcEnd.QuadPart -= qpcStart.QuadPart;

	LARGE_INTEGER sum;
	sum.QuadPart = 0;
	for(size_t i = 0; i <  m_FilesInf.size(); ++i)
	{
		sum.QuadPart +=  m_FilesInf[i]->Size;
	}

	TCHAR buf[128];
	::QueryPerformanceFrequency(&freq);
	double dt = (1.0*qpcEnd.QuadPart/(double)freq.QuadPart);
	_sntprintf_s((TCHAR*)buf, ARRAYSIZE(buf), _TRUNCATE, _T("%s time=%.3f \t %.2f MB/s"), szDescr, dt,(sum.QuadPart >> 20)/dt);
	::SendMessage(hWnd, UM_FILE_INF_STATUS, 0, (LPARAM)buf);
	//OutputDebugString(_T("PerformanceShow\n"));
}

	DWORD SaveToFile(CFilesInf &filesInf, TCHAR* filePath)
	{
	HANDLE hFile = NULL;
	DWORD dwErrorCode = NO_ERROR;
	try
	{
		// Open the file
		hFile = CreateFile(filePath,
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return GetLastError();

		CFileInfFormatter formatter;
		DWORD dwWritten;
		for(size_t i = 0; i < filesInf.m_FilesInf.size(); ++i )
		{
			formatter.Format(filesInf.m_FilesInf[i]);
			formatter.ChPopBack();
			formatter.ChPushBack('\n');
			//formatter.ChPushBack(NULL);
			WriteFile(hFile, formatter.Buf, formatter.Len << L2SZCH, &dwWritten, NULL);
		}
	}
	catch(...)
	{
		// An unknown exception happened
		dwErrorCode = ERROR_CRC;
	}

	if (hFile != NULL)
		CloseHandle(hFile);
		
	return dwErrorCode;
}
