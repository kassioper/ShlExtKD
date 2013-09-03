#pragma once
#include "Common.h"
#include "windows.h"


typedef DWORD (*pFnFileDetailsRead)(
	  const TCHAR* filePath
	, QWORD&  qwFileSize
	, FILETIME& fileTime
	, DWORD& dwCheckSum
	, HANDLE evAbort
);


  //DWORD FileDetailsRead(const TCHAR* filePath, QWORD&  qwFileSize, FILETIME& fileTime, DWORD& dwCheckSum, bool volatile * pisAbort);
	DWORD FileDetailsReadStd(const TCHAR* filePath, QWORD&  qwSize, FILETIME& fileTime, DWORD& dwCheckSum
		, HANDLE evAbort);

  DWORD FileDetailsReadMap(const TCHAR* filePath, QWORD&  qwSize, FILETIME& fileTime, DWORD& dwCheckSum
		, HANDLE evAbort);

	bool FileNameCompare(const tstring *strL, const tstring *strR);

