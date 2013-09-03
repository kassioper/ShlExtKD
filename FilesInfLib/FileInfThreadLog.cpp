#include "StdAfx.h"
#include "windows.h"
#include "FileInfThreads.h"
#include "FilesInf.h"
#include "FileInfFormatter.h"

DWORD WINAPI FilesInfThreadFuncLog( LPVOID lpParam ) 
{
	//FileDetailsLaunchThreadArgs& args = *(FileDetailsLaunchThreadArgs*)lpParam;
	//OutputDebugString(_T("Log Beg\n"));
	HANDLE hFile = NULL;
	DWORD dwErrorCode = NO_ERROR;
 
	CFilesInf& filesInf = *(CFilesInf*)lpParam;
	//{
	//	IMalloc *pIMalloc;
	//	HRESULT hr = SHGetMalloc(&pIMalloc);
	//	if (SUCCEEDED(hr))
	//	{
	//		pIMalloc->Free(lpParam);
	//		pIMalloc->Release();
	//	}
	//}
  CFileInfFormatter  formatter;	

	hFile = CreateFile(formatter.FormatLogFileName(&filesInf),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwErrorCode = GetLastError();
		goto lbEnd;
	}

	DWORD dwWritten;
	HANDLE evs[2];
	evs[0] = filesInf.m_evItemComplete;
	evs[1] = filesInf.m_evAbort;
	while (true)
	{
		//OutputDebugString(_T("WaitForMultipleObjects Before\n"));
		//DWORD waitRez = WaitForSingleObject(filesInf.m_evItemComplete, INFINITE);
		if (filesInf.m_FilesInf.size() <= filesInf.m_LogOutFirstItem)
			goto lbEnd;

		DWORD waitRez = WaitForMultipleObjects(ARRAYSIZE(evs), evs, FALSE, INFINITE);
		//Sleep(100);
		//DWORD waitRez = WAIT_OBJECT_0;
		//OutputDebugString(_T("WaitForMultipleObjects After\n"));
  	//goto lbEnd;
		switch (waitRez) {
			case WAIT_OBJECT_0:
				{
					while (true) // optimization
					{
						if (filesInf.m_FilesInf.size() <= filesInf.m_LogOutFirstItem)
							goto lbEnd;
						CFileInf &fileInf = *filesInf.m_FilesInf[filesInf.m_LogOutFirstItem];
						if (false == fileInf.IsNotRead())
						{
							formatter.Format(&fileInf);
							formatter.ChPopBack();
							formatter.ChPushBack('\r');
							formatter.ChPushBack('\n');
							//OutputDebugString(_T("WriteFile Before\n"));
							WriteFile(hFile, formatter.Buf, formatter.Len << L2SZCH, &dwWritten, NULL);
							
							++filesInf.m_LogOutFirstItem; // can be not interlocked
						}
					  else 
					    break; // break inner while
					} // inner while
				}
				break;
			//case WAIT_0BJECT_0 + 1:
			//case WAIT_TIMEOUT:
			//case WAIT_FAILED:
			default:
				OutputDebugString(_T("Log Abort\n"));
				goto lbEnd;
				break;
		}
		//w wait for Abort
	} // while (true)
lbEnd:
	OutputDebugString(_T("Log End\n"));
	if (hFile != NULL)
	{
		FlushFileBuffers(hFile);
		CloseHandle(hFile);
	}
	filesInf.OnThreadEnd();
//w dec
	return TRUE;
}