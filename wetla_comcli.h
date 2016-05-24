#ifndef __WETLA_COMCLI_H__
#define __WETLA_COMCLI_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCOMCLI_H__
	#error wetla_host.h requires atlcomcli.h to be included first
#endif


namespace WETLA
{

template <class T>
class CCreateLicBase
{
public:
	HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL, BSTR bstrLicKey) throw()
	{
		ATLASSERT(p == NULL);
		CComPtr<IClassFactory2> spCF;
		HRESULT hr = ::CoGetClassObject(rclsid, dwClsContext, NULL, 
			IID_IClassFactory2, reinterpret_cast<void**>(&spCF));

		return SUCCEEDED(hr) ? spCF->CreateInstanceLic(pUnkOuter, NULL, __uuidof(T),
			bstrLicKey, reinterpret_cast<LPVOID*>(&p)) : hr;
	}
	HRESULT CoCreateInstance(LPCOLESTR szProgID, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL, BSTR bstrLicKey) throw()
	{
		CLSID clsid;
		ATLASSERT(p == NULL);
		HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
		return (SUCCEEDED(hr) ? CoCreateInstance(clsid, pUnkOuter, dwClsContext, bstrLicKey) : hr;
	}
	HRESULT RequestLicKey(BSTR* pbstrLicKey)
	{
		ATLASSERT(p != NULL && pbstrLicKey != NULL);
		CComPtr<IClassFactory2> spCF;
		HRESULT hr = p->QueryInterface(__uuidof(IClassFactory2), reinterpret_cast<LPVOID*>(&spCF));
		return SUCCEEDED(hr) ? spCF->RequestLicKey(NULL, pbstrLicKey) : hr;
	}
};

template <class T>
class CComLicPtr : public CComPtr<T>, public CCreateLicBase<T>
{
};

template <class T, const IID* piid>
class CComLicQIPtr : public CComQIPtr<T, piid>, public CCreateLicBase<T>
{
};

}; //namespace WETLA

#endif // __WETLA_COMCLI_H__
