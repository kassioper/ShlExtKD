#include "StdAfx.h"
#include "CoShlExtKDFactory.h"
#include "ShlExtKD.h"

extern DWORD g_lockCount;
extern DWORD g_objCount;

CoShlExtKDFactory::CoShlExtKDFactory(void)
{
	m_refCount = 0;
	g_objCount++;
}

CoShlExtKDFactory::~CoShlExtKDFactory(void)
{
	g_objCount--;
}


// IUnknown
STDMETHODIMP_(ULONG) CoShlExtKDFactory::AddRef()
{
	return ++m_refCount;

}

STDMETHODIMP_(ULONG) CoShlExtKDFactory::Release()
{
	if(--m_refCount == 0)
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

STDMETHODIMP CoShlExtKDFactory::QueryInterface(REFIID riid, void** ppv)
{
	// Which aspect of me do they want?
	if(riid == IID_IUnknown)
	{
		*ppv = (IUnknown*)this;
	}
	else if(riid == IID_IClassFactory)
	{
		*ppv = (IClassFactory*)this;
	}	
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	((IUnknown*)(*ppv))->AddRef();
	return S_OK;
}


// ICF
STDMETHODIMP CoShlExtKDFactory::LockServer(BOOL fLock)
{
	if(fLock)
		++g_lockCount;
	else
		--g_lockCount;

	return S_OK;
}

STDMETHODIMP CoShlExtKDFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void** ppv)
{
	// We do not support aggregation in this class object.
	if(pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	ShlExtKD* pCarObj = NULL;
	HRESULT hr;

	// Create the car.
	pCarObj = new ShlExtKD;
	
	// Ask car for an interface.
	hr = pCarObj -> QueryInterface(riid, ppv);

	// Problem?  We must delete the memory we allocated.
	if (FAILED(hr))
		delete pCarObj;

	return hr;	// S_OK or E_NOINTERFACE.
	
}

