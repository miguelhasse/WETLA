#ifndef __WETLA_DISPATCH_H__
#define __WETLA_DISPATCH_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCOM_H__
	#error wetla_dispatch.h requires atlcom.h to be included first
#endif

namespace WETLA
{

/////////////////////////////////////////////////////////////////////////////
// Dynamic IDispatch macros
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001 Bjarke Viksoe.

// Parameter types: by value VTs
#define VTS_EMPTY           L"\x00"
#define VTS_I2              L"\x02"      // a 'short'
#define VTS_I4              L"\x03"      // a 'long'
#define VTS_R4              L"\x04"      // a 'float'
#define VTS_R8              L"\x05"      // a 'double'
#define VTS_CY              L"\x06"      // a 'CY'
#define VTS_DATE            L"\x07"      // a 'DATE'
#define VTS_BSTR            L"\x08"      // a 'BSTR'
#define VTS_DISPATCH        L"\x09"      // an 'IDispatch*'
#define VTS_SCODE           L"\x0A"      // a 'SCODE'
#define VTS_BOOL            L"\x0B"      // a 'BOOL'
#define VTS_VARIANT         L"\x0C"      // a 'VARIANT'
#define VTS_UNKNOWN         L"\x0D"      // an 'IUnknown*'

// Parameter types: by reference VTs
#define VTS_REF_I2          L"\x4002"    // a 'short*'
#define VTS_REF_I4          L"\x4003"    // a 'long*'
#define VTS_REF_R4          L"\x4004"    // a 'float*'
#define VTS_REF_R8          L"\x4005"    // a 'double*'
#define VTS_REF_CY          L"\x4006"    // a 'CY*'
#define VTS_REF_DATE        L"\x4007"    // a 'DATE*'
#define VTS_REF_BSTR        L"\x4008"    // a 'BSTR*'
#define VTS_REF_DISPATCH    L"\x4009"    // an 'IDispatch**'
#define VTS_REF_SCODE       L"\x400A"    // a 'SCODE*'
#define VTS_REF_BOOL        L"\x400B"    // a 'VARIANT_BOOL*'
#define VTS_REF_VARIANT     L"\x400C"    // a 'VARIANT*'
#define VTS_REF_UNKNOWN     L"\x400D"    // an 'IUnknown**'


template <class T>
struct _ATL_DISPATCH_ENTRY
{
	OLECHAR *szName;
	DISPID dispid;
	UINT wFlags;
	VARTYPE vtReturn;
	UINT nArgs;
	VARTYPE vtArgs[_ATL_MAX_VARTYPES];
	void (__stdcall T::*pfn)();
};

#define BEGIN_DISPATCH_MAP(theClass) \
	static const _ATL_DISPATCH_ENTRY<theClass>* _GetDispMap()\
	{\
		typedef theClass _atl_disp_classtype;\
		static const _ATL_DISPATCH_ENTRY<_atl_disp_classtype> _dispmap[] = {

#define DISP_METHOD_ID(func, dispid, vtRet, nCnt, vtArgs) \
	{ OLESTR(#func), dispid, DISPATCH_METHOD, vtRet, nCnt, {vtArgs}, (void (__stdcall _atl_disp_classtype::*)())func },
#define DISP_METHOD(func, vtRet, nCnt, vtArgs) \
	DISP_METHOD_ID(func, DISPID_UNKNOWN, vtRet, nCnt, vtArgs)

#define DISP_METHOD0_ID(func, dispid, vtRet) \
	{ OLESTR(#func), dispid, DISPATCH_METHOD, vtRet, 0, {VT_EMPTY}, (void (__stdcall _atl_disp_classtype::*)())func },
#define DISP_METHOD0(func, vtRet) \
	DISP_METHOD0_ID(func, DISPID_UNKNOWN, vtRet)

#define DISP_METHOD1_ID(func, dispid, vtRet, vtArg) \
	{ OLESTR(#func), dispid, DISPATCH_METHOD, vtRet, 1, {vtArg}, (void (__stdcall _atl_disp_classtype::*)())func },
#define DISP_METHOD1(func, vtRet, vtArg) \
	DISP_METHOD1_ID(func, DISPID_UNKNOWN, vtRet, vtArg)

#define DISP_PROP_ID(member, dispid, vt) \
	{ OLESTR(#member), dispid, DISPATCH_PROPERTYGET, vt, 0, {VT_EMPTY}, (void (__stdcall _atl_disp_classtype::*)())get_##member }, \
	{ OLESTR(#member), DISPID_PROPERTYPUT, DISPATCH_PROPERTYPUT, VT_EMPTY, 1, {vt}, (void (__stdcall _atl_disp_classtype::*)())put_##member },
#define DISP_PROP(member, vt) \
	DISP_PROP_ID(member, DISPID_UNKNOWN, vt)

#define DISP_PROPGET_ID(member, dispid, vt) \
	{ OLESTR(#member), dispid, DISPATCH_PROPERTYGET, vt, 0, {VT_EMPTY}, (void (__stdcall _atl_disp_classtype::*)())get_##member },
#define DISP_PROPGET(member, vt) \
	DISP_PROPGET_ID(member, DISPID_UNKNOWN, vt)

#define DISP_PROPPUT_ID(member, dispid, vt) \
	{ OLESTR(#member), dispid, DISPATCH_PROPERTYPUT, VT_EMPTY, 1, {vt}, (void (__stdcall _atl_disp_classtype::*)())put_##member },
#define DISP_PROPPUT(member, vt) \
	DISP_PROPPUT_ID(member, DISPID_UNKNOWN, vt)

#define END_DISPATCH_MAP() {0} }; return _dispmap; }

/////////////////////////////////////////////////////////////////////////////
// CComDynTypeInfoHolder
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001 Bjarke Viksoe.

template <class	T>
class CComDynTypeInfoHolder
{
	// Should be 'protected' but can cause compiler	to generate	fat	code.
public:
	const GUID*	m_pguid;
	const GUID*	m_plibid;
	WORD m_wMajor;
	WORD m_wMinor;

	ITypeInfo* m_pInfo;
	long m_dwRef;
	struct stringdispid
	{
		CComBSTR bstr;
		int	nLen;
		DISPID id;
		stringdispid() : nLen(0), id(DISPID_UNKNOWN){}
	};
	stringdispid* m_pMap;
	int	m_nCount;

	// The above members must be present in	that order because they
	// are initialized by ATL (as a	static member)
	INTERFACEDATA* m_pITF;

public:
	HRESULT	GetTI(LCID lcid, ITypeInfo** ppInfo)
	{
		HRESULT	hr = S_OK;
		if (m_pInfo	== NULL)
			hr = GetTI(lcid);

		if ((*ppInfo = m_pInfo)	!= NULL)
		{
			m_pInfo->AddRef();
			hr = S_OK;
		}
		return hr;
	}
	HRESULT	GetTI(LCID lcid);
	HRESULT	EnsureTI(LCID lcid)
	{
		return (m_pInfo	== NULL) ? GetTI(lcid) : S_OK;
	}
	// This	function is	called by the module on	exit
	// It is registered	through	_Module.AddTermFunc()
	static void	__stdcall Cleanup(DWORD	dw)
	{
		CComDynTypeInfoHolder* p = (CComDynTypeInfoHolder*)dw;
		if (p->m_pInfo != NULL)
		{
			p->m_pInfo->Release();
			p->m_pInfo = NULL;
			// Release internal	interface block
			for	(UINT i	= 0; i < p->m_pITF->cMembers; i++)
				delete [] p->m_pITF->pmethdata[i].ppdata;

			delete [] p->m_pITF->pmethdata;
			delete p->m_pITF;
		}
		if (p->m_pMap != NULL)
		{
			delete [] p->m_pMap;
			p->m_pMap =	NULL;
		}
	}
	HRESULT	GetTypeInfo(UINT /*itinfo*/, LCID lcid,	ITypeInfo**	pptinfo)
	{
		return (pptinfo	!= NULL) ? GetTI(lcid, pptinfo) : E_POINTER;
	}
	HRESULT	GetIDsOfNames(REFIID /*riid*/, LPOLESTR* rgszNames,	UINT cNames, LCID lcid,	DISPID*	rgdispid)
	{
		HRESULT	hr = EnsureTI(lcid);
		if (m_pInfo	!= NULL)
		{
			for	(int i = 0;	i <	(int)cNames; i++)
			{
				int n = ocslen(rgszNames[i]);
				for (int j	= 0; j < m_nCount; j++)
				{
					if	((n	== m_pMap[j].nLen) && memcmp(m_pMap[j].bstr,
						rgszNames[i], m_pMap[j].nLen * sizeof(OLECHAR)) == 0)
					{
						rgdispid[i] = m_pMap[j].id;
						break;
					}
				}
				if	(j == m_nCount)
				{
					if	(FAILED(hr = m_pInfo->GetIDsOfNames(rgszNames, cNames, rgdispid)))
						break;
				}
			}
		}
		return hr;
	}
	HRESULT	Invoke(IDispatch* p, DISPID	dispidMember, REFIID /*riid*/,
		LCID lcid, WORD	wFlags,	DISPPARAMS*	pdispparams, VARIANT* pvarResult,
		EXCEPINFO* /*pexcepinfo*/, UINT* /*puArgErr*/)
	{
		HRESULT	hr = EnsureTI(lcid);
		if (m_pInfo	!= NULL)
		{
			if ((wFlags	!= DISPATCH_PROPERTYPUT) &&	(pdispparams->cNamedArgs > 0))
				return DISP_E_NONAMEDARGS;

			const _ATL_DISPATCH_ENTRY<T>* pMap = T::_GetDispMap();
			DISPID i = 1;

			while (pMap->pfn !=	NULL)
			{
				DISPID dispid = (pMap->dispid == DISPID_UNKNOWN) ? i :	pMap->dispid;

				if (dispidMember == dispid)
				{
					if ((wFlags == DISPATCH_PROPERTYPUT) && 
						((pMap + 1)->dispid == DISPID_PROPERTYPUT)) 
						pMap++;

					UINT nArgs = pMap->nArgs;
					if (pdispparams->cArgs != nArgs)
						return	DISP_E_BADPARAMCOUNT;

					VARIANTARG** pVarArgs = nArgs ? 
						(VARIANTARG**)_alloca(sizeof(VARIANTARG*)*nArgs) :	NULL;

					for (UINT i = 0; i	< nArgs; i++)
					{
						pVarArgs[i] = &pdispparams->rgvarg[nArgs - i - 1];
						if (pMap->vtArgs[i] != pVarArgs[i]->vt)
							return DISP_E_TYPEMISMATCH;
					}

					T *pT = static_cast<T*>(p);
					CComStdCallThunk<T> thunk;
					thunk.Init(pMap->pfn, pT);

					CComVariant tmpResult;
					if (pvarResult == NULL)
						pvarResult = &tmpResult;

					return ::DispCallFunc(&thunk, 0, CC_STDCALL,
						pMap->vtReturn, nArgs, (VARTYPE *)pMap->vtArgs, pVarArgs, pvarResult);
				}
				i++;
				pMap++;
			}
			return	DISP_E_MEMBERNOTFOUND;
		}
		return hr;
	}
	HRESULT	LoadNameCache(ITypeInfo* /*pTypeInfo*/)
	{
		// A name cache	is really not useful here since	we already have
		// a fine map of the names.	Oh well...
		const _ATL_DISPATCH_ENTRY<T>* pTemp, *pMap = T::_GetDispMap();
		pTemp =	pMap;
		m_nCount = 0;	   
		while (pTemp->pfn != NULL)
		{
			m_nCount++;
			pTemp++;
		}
		m_pMap = (m_nCount == 0) ? NULL	: new stringdispid[m_nCount];
		DISPID i = 0;
		while (pMap->pfn !=	NULL)
		{
			m_pMap[i].bstr = pMap->szName;
			m_pMap[i].nLen = ::SysStringLen(m_pMap[i].bstr);
			m_pMap[i].id = (pMap->dispid ==	DISPID_UNKNOWN)	? (i + 1) :	pMap->dispid;
			// NOTE: In	case of	DISPID_PROPERTYPUT we assume that a	name entry 
			// exists before it, which will	have the correct DISPID.
			pMap++;
			i++;
		}
		return S_OK;
	}
};

template <class	T>
inline HRESULT CComDynTypeInfoHolder<T>::GetTI(LCID	lcid)
{	
	const _ATL_DISPATCH_ENTRY<T>* pTemp, *pMap = T::_GetDispMap();
	pTemp =	pMap;
	UINT nArgs = 0;
	while (pTemp->pfn != NULL)
	{
		nArgs++;
		pTemp++;
	}
	// Create INTERFACEDATA	structures. They cannot be created on the stack.
	// Seems that the ancient CreateDispTypeInfo() method uses weak references.
	m_pITF = new INTERFACEDATA;
	m_pITF->cMembers = nArgs;
	METHODDATA*	pM = m_pITF->pmethdata = new METHODDATA[nArgs];
	DISPID dispid =	1;
	DISPID oldid = 0;
	LONG i = 0;	// Method index

	while (pMap->pfn !=	NULL)
	{
		pM->szName = pMap->szName;
		pM->dispid = (pMap->dispid == DISPID_UNKNOWN) ?	dispid : pMap->dispid;

		if (pMap->dispid ==	DISPID_PROPERTYPUT)	
			pM->dispid = oldid;	// Support hack	in our DISP_PROP macro

		pM->iMeth =	i;
		pM->cc = CC_STDCALL;
		pM->ppdata = NULL;
		pM->cArgs =	pMap->nArgs;

		if (pMap->nArgs	> 0)
		{
			PARAMDATA* pParams = pM->ppdata	= new PARAMDATA[pMap->nArgs];

			for	(UINT j	= 0; j < pMap->nArgs; j++)
			{
				pParams->szName	= NULL;
				pParams->vt	= pMap->vtArgs[j];
				pParams++;
			}
		}
		pM->wFlags = (USHORT) pMap->wFlags;
		pM->vtReturn = pMap->vtReturn;

		// Next	entry...
		oldid =	pM->dispid;
		dispid++;
		i++;
		pM++;
		pMap++;
	}
	HRESULT	hr = ::CreateDispTypeInfo(m_pITF, lcid,	&m_pInfo);
	// The result of CreateDispTypeInfo() is very disappointing. It
	// doesn't create a	full ITypeInfo object, so calling ITypeInfo::GetTypeAttr()
	// claims that there are NO	methods	and	properties in the list (?).
	// This	basically renders the idea of creating a CComTypeInfoHolder	clone
	// useless.	I might	as well	have done a	better IDispatch impl instead.
	if (SUCCEEDED(hr))
	{
		LoadNameCache(m_pInfo);
		_Module.AddTermFunc(Cleanup, (DWORD)this);
	}
	return hr;
};

}; //namespace WETLA

#endif // __WETLA_DISPATCH_H__
