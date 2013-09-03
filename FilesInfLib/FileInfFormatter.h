#pragma once
#include "stdafx.h"
#include "windows.h"
//#include "Common.h"
#include "FileInf.h"
#include "FilesInf.h"

class CFileInfFormatter
{	
public:
	TCHAR Buf[MAX_PATH+128];
	size_t Len;

	CFileInfFormatter()
	{
		Len = 0;
		Buf[0] = NULL;
	}

	CFileInfFormatter(CFileInf* pFileInf)
	{
		Format(pFileInf);
	}

	TCHAR* Format(CFileInf* pFileInf);
TCHAR* FormatLogFileName(CFilesInf* pFilesInf, TCHAR **pszFileApart = NULL);

	void ChPopBack()
	{
		_ASSERTE(Len);
		--Len;		
	}

	void ChPushBack(TCHAR ch)
	{
		Buf[Len++] = ch;		
		_ASSERTE(Len < ARRAYSIZE(Buf));
	}

};
