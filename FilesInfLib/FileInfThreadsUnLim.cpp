#include "StdAfx.h"
#include "FileInfThreads.h"
#include "FilesInf.h"
#include "FilesUtils.h"
#include "windows.h"
#include "Shlobj.h"

struct FileDetailsReadThreadArgs
{
	CFilesInf* filesInf;
	size_t itemID;
	HWND	hWnd;
};

DWORD WINAPI FileDetailsReadThreadFunc( LPVOID lpParam ) 
{
	FileDetailsReadThreadArgs& args = *(FileDetailsReadThreadArgs*)lpParam;
	size_t itemID = args.itemID;
	HWND	hWnd = args.hWnd;

	CFilesInf* filesInf = (CFilesInf*)args.filesInf;
	{
		IMalloc *pIMalloc;
		HRESULT hr = SHGetMalloc(&pIMalloc);
		if (SUCCEEDED(hr))
		{
			pIMalloc->Free(lpParam);
			pIMalloc->Release();
		}
	}

	if (WaitForSingleObject(filesInf->m_evAbort, 0) == WAIT_TIMEOUT)
	{
		filesInf->FileDetailsRead(itemID);

		::PostMessage(hWnd, UM_FILE_INF_ITEM_REFRESH, 0, itemID);

		SetEvent(filesInf->m_evItemComplete);
		if (filesInf->m_FilesInf.size() <= itemID + 1)
		{
			filesInf->PerformanceShow(hWnd);
		}
	}
	
	filesInf->OnThreadEnd();
	return 0;
}

DWORD WINAPI FilesInfThreadFuncUnlim( LPVOID lpParam ) 
{
	FileDetailsLaunchThreadArgs& args = *(FileDetailsLaunchThreadArgs*)lpParam;

	CFilesInf* filesInf = (CFilesInf*)args.filesInf;
	HWND	hWnd = (HWND)args.hWnd;
	
	IMalloc *pIMalloc;
	HRESULT hr = SHGetMalloc(&pIMalloc);
	if (FAILED(hr))
		return 0;
	
	pIMalloc->Free(lpParam);
	pIMalloc->Release();
	
	CFilesInf::Index count = filesInf->m_FilesInf.size();
	for(CFilesInf::Index i = 0; i < count; ++i  )
	{
		//if (filesInf->isAbort)
		if (WaitForSingleObject(filesInf->m_evAbort, 0) != WAIT_TIMEOUT)
			break;

		FileDetailsReadThreadArgs* pArgs = 
		(FileDetailsReadThreadArgs*) pIMalloc->Alloc(
								sizeof(FileDetailsReadThreadArgs));

		pArgs->filesInf = filesInf;
		pArgs->itemID = i;
		pArgs->hWnd = hWnd;

		InterlockedIncrement(&filesInf->m_ThreadCount);

		QueueUserWorkItem( FileDetailsReadThreadFunc, (PVOID) pArgs, WT_EXECUTEDEFAULT );

	} // for 
	pIMalloc->Release();

	filesInf->OnThreadEnd();

	return TRUE;
}
