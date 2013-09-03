#pragma once
#ifndef __SHLEXTKD_H_
#define __SHLEXTKD_H_

#include <shellAPI.h>
#include <shlobj.h>
#include "ShellExt_D_h.h"
#include "FilesInf.h"

class ShlExtKD : public IContextMenu, public IShellExtInit
{
public:
	ShlExtKD(void);
	virtual ~ShlExtKD(void);

		// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** pIFace);
	STDMETHODIMP_(DWORD)AddRef();
	STDMETHODIMP_(DWORD)Release();

    // IShellExtInit
    STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
    STDMETHODIMP GetCommandString(UINT_PTR, UINT, UINT*, LPSTR, UINT);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
    STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

protected:
  	DWORD	m_refCount;
    TCHAR m_szFile [MAX_PATH];
    TCHAR m_szFiles [MAX_PATH * 16];
		CFilesInf m_FilesInf;
};
#endif //__SHLEXTKD_H_
