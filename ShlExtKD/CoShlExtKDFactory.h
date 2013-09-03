#pragma once

class CoShlExtKDFactory : public IClassFactory 
{
public:
	CoShlExtKDFactory(void);
	virtual  ~CoShlExtKDFactory(void);

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** pIFace);
	STDMETHODIMP_(DWORD)AddRef();
	STDMETHODIMP_(DWORD)Release();

	// ICF
	STDMETHODIMP LockServer(BOOL fLock);
	STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void** ppv);

private:
	DWORD	m_refCount;

};
