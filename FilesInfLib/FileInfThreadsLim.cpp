#include "StdAfx.h"
#include "FileInfThreads.h"
#include "FilesInf.h"
#include "FilesUtils.h"
#include "windows.h"
#include "Shlobj.h"

DWORD WINAPI FilesInfThreadFuncLim( LPVOID lpParam ) 
{
	FileDetailsLaunchThreadArgs& args = *(FileDetailsLaunchThreadArgs*)lpParam;

	CFilesInf& filesInf = *(CFilesInf*)args.filesInf;
	HWND	hWnd = (HWND)args.hWnd;
	{
		IMalloc *pIMalloc;
		HRESULT hr = SHGetMalloc(&pIMalloc);
		if (SUCCEEDED(hr))
		{
			pIMalloc->Free(lpParam);
			pIMalloc->Release();
		}
	}

	size_t count = filesInf.m_FilesInf.size();
	while(true)
	{
		//if (!filesInf.isAbort)
		if (WaitForSingleObject(filesInf.m_evAbort, 0) != WAIT_TIMEOUT)
			break;

		size_t itemID = (size_t)InterlockedIncrement(&filesInf.m_ThreadItemsCount) - 1;
		if (itemID >= count)
		{
			filesInf.PerformanceShow(hWnd);
			break;
		}

		filesInf.FileDetailsRead(itemID);

		//CFileInf* fileInf = filesInf.m_FilesInf[itemID];
		//fileInf->DetailsRead(&filesInf.isAbort);

		::PostMessage(hWnd, UM_FILE_INF_ITEM_REFRESH, 0, itemID);
		SetEvent(filesInf.m_evItemComplete);

	}

	//PerformanceShow(qpcStart, hWnd, filesInf);
	filesInf.OnThreadEnd();
	return TRUE;
}
