// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "CoShlExtKDFactory.h"
#include <shlobj.h>
#include "ShellExt_D_h.h"

HMODULE g_hModule;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hModule = hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

// Need these global data points to figure
// out when the DLL can get unloaded.
ULONG g_lockCount = 0;
ULONG g_objCount = 0;


// Called by SCM and CoFreeUnusedLibraries() to see
// if this DLL can be removed from memory.
STDAPI DllCanUnloadNow()
{
	if(g_lockCount == 0 && g_objCount == 0)
	{
		//MessageBox(NULL, _T("Goodbye from DllCanUnloadNow!"), _T("CarInProcServer"), MB_OK | MB_SETFOREGROUND);
		return S_OK;		// Unload me.	
	}
	else
		return S_FALSE;		// Keep me alive.
}


// Called by SCM in responce to CoGetClassObject() or
// CoCreateInstance().  Creates a given class factory for
// the client based on the CLSID of the coclass.
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
	//MessageBox(NULL, _T("Hello from DllGetClassObject!"), _T("CarInProcServer"), MB_OK | MB_SETFOREGROUND);
	HRESULT hr;
	CoShlExtKDFactory *pCFact = NULL;
	
	// We only know how to make CoHexagon objects in this house.
	//if(false == 
	//		((IID_IContextMenu == rclsid)
	//	||(IID_IShellExtInit == rclsid)
	//	||(IID_IUnknown == rclsid)
	//	||(IID_IShlExtKD == rclsid)
	//	)
	//	)

	if(CLSID_ShlExtKD != rclsid)
		return CLASS_E_CLASSNOTAVAILABLE;
 
	// They want a CoCarClassFactory
	pCFact = new CoShlExtKDFactory;	

	// Go get the interface from the CoCarClassFactory
	hr = pCFact -> QueryInterface(riid, ppv);

	if(FAILED(hr))
		delete pCFact;
	
	return hr;
//		return S_OK;		// Unload me.	
}

