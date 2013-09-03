#pragma once
#include "windows.h"
//#include "stdafx.h"
#include <vector>
#include "FileInf.h"

// message about compliting file details reading
#define UM_FILE_INF_ITEM_REFRESH WM_USER + 1
#define UM_FILE_INF_STATUS			 WM_USER + 2

enum FilesInfMode
{
	FilesInfMode_DEFAULT = 0,
	FilesInfMode_FILE_MAPPING = 1 << 0, // using file mapping for reading file
};

// Front-end class for collect and show files details information
class CFilesInf
{
	void FileAdd(tstring *path);
	FilesInfMode mode;
	size_t threadsMaxCount; // 0 - unlimited

	// File Details Calc task fields:
	TCHAR szDescr[64];
	LARGE_INTEGER qpcStart;

	void Init();
public:
	typedef std::vector<CFileInf*>::size_type Index;

	std::vector<CFileInf*> m_FilesInf;
	// abort flag
	//bool volatile isAbort;
	// number of all threads
	LONG volatile m_ThreadCount;
	// First file without detail (for FilesInfThreadFuncLim)
	LONG volatile m_ThreadItemsCount;
	// First file not in log
	size_t volatile m_LogOutFirstItem; // can be not volatile 
	HANDLE m_evAbort;
	HANDLE m_evItemComplete; 
	HANDLE m_evFinish;

	CFilesInf():threadsMaxCount(0), mode(FilesInfMode_DEFAULT) { Init(); }
	CFilesInf(size_t threadsMaxCount, FilesInfMode mode)
		:threadsMaxCount(threadsMaxCount), mode(mode) { Init(); }
	~CFilesInf(void); 

	// Add File to collection
	void FileAdd(TCHAR *path);
	// Clear collection
	void Clear(void);
	// Show DialogBox with file details information:
	// HINSTANCE dll hInstance
	// hWnd - parent window
	INT_PTR FilesInfViewDlgShow(HINSTANCE hInstance, HWND hWnd);
	// Begin files details reading
	BOOL FilesDetailsRead(HWND hWnd);
	// Stop files details reading
	void FilesDetailsReadStop();

	// Show perforance message at the finish of processing
	void PerformanceShow(HWND hWnd);
	void OnThreadEnd();

	void FileDetailsRead(size_t itmeID);

#ifdef _DEBUG
	// design-time method
	int Debug(int n = 0);
#endif
};

DWORD SaveToFile(CFilesInf &filesInf, TCHAR* filePath);


//typedef std::vector<CFileInf*>::size_type FileInfIndex;


