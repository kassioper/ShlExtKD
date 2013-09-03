#pragma once
#include <windows.h>
#include "FilesInf.h"

struct FileDetailsLaunchThreadArgs
{
	CFilesInf* filesInf;
	HWND	hWnd;
	//LARGE_INTEGER qpcStart;
};

DWORD WINAPI FilesInfThreadFuncUnlim( LPVOID lpParam);

DWORD WINAPI FilesInfThreadFuncLim( LPVOID lpParam);

DWORD WINAPI FilesInfThreadFuncLog( LPVOID lpParam );
