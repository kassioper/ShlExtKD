#pragma once
#include "stdafx.h"
#include "windows.h"
#include "Common.h"

class CFileInf
{

public:
	tstring *Path;
	QWORD  Size;
	FILETIME	TimeCreate;
	DWORD			CheckSum; // if Size = 0 and CheckSum = 1 - error reading file details

	CFileInf(tstring *path)
	{
		Path = path;
		Size = 0;
		TimeCreate.dwLowDateTime = 0;
		TimeCreate.dwHighDateTime = 0;
		CheckSum = 0;
	}

	~CFileInf(void)
	{
		delete Path;
	}

	bool IsNotRead()
	{
		return 							 
				 0 == Size 
			&& 0 == TimeCreate.dwLowDateTime
      && 0 == TimeCreate.dwHighDateTime
      && 0 == CheckSum // in case error
		;
	}
};

