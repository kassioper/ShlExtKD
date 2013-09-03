#pragma once
#include <windows.h>
#include "FilesInf.h"

// Show DialogBox with file details information:
INT_PTR FileInfViewDlgShow(HINSTANCE hInstance, HWND hWnd, CFilesInf* pFilesInfData);
