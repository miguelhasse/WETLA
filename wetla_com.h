#ifndef __WETLA_COM_H__
#define __WETLA_COM_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCOM_H__
	#error wetla_com.h requires atlcom.h to be included first
#endif

namespace WETLA
{

template <class Base> // Base must be derived from CComObjectRoot
class CComStackObject : public Base
{
public:
	typedef Base _BaseClass;

	CComStackObject(void* = NULL) 
	{ 
		m_dwRef = 1;
		m_hResFinalConstruct = FinalConstruct(); 
	}
	virtual ~CComStackObject()
	{
		ATLASSERT(m_dwRef == 1);
		FinalRelease();
#ifdef _ATL_DEBUG_INTERFACES
		_AtlDebugInterfacesModule.DeleteNonAddRefThunk(_GetRawUnknown());
#endif
	}
	STDMETHOD_(ULONG, AddRef)()
	{
#ifdef _DEBUG
		return InternalAddRef();
#else
		return m_dwRef;
#endif
	}
	STDMETHOD_(ULONG, Release)()
	{
#ifdef _DEBUG
		return InternalRelease();
#else
		return m_dwRef;
#endif
	}
	STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject)
	{
		return _InternalQueryInterface(iid, ppvObject);
	}
	HRESULT m_hResFinalConstruct;
};

template <class Base> // Base must be derived from CComObjectRoot
class CComEmbeddedObject : public Base
{
public:
	typedef Base _BaseClass;

	CComEmbeddedObject(void* pv = NULL)
	{
		m_pOuterUnknown = (IUnknown*)pv;
		m_hResFinalConstruct = FinalConstruct(); 
	}
	virtual ~CComEmbeddedObject()
	{
		FinalRelease();
#ifdef _ATL_DEBUG_INTERFACES
		_AtlDebugInterfacesModule.DeleteNonAddRefThunk(_GetRawUnknown());
		_AtlDebugInterfacesModule.DeleteNonAddRefThunk(m_pOuterUnknown);
#endif
	}
	STDMETHOD_(ULONG, AddRef)()
	{
		ATLASSERT(m_pOuterUnknown);
		return OuterAddRef();
	}
	STDMETHOD_(ULONG, Release)()
	{
		ATLASSERT(m_pOuterUnknown);
		return OuterRelease();
	}
	STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject)
	{
		return _InternalQueryInterface(iid, ppvObject);
	}
	template <class Q>
	HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp)
	{
		return QueryInterface(__uuidof(Q), (void**)pp);
	}
	void SetOuterUnknown(IUnknown *pContainerUnknown)
	{
		m_pOuterUnknown = pContainerUnknown;
	}
	// GetControllingUnknown may be virtual so define it here
	IUnknown* GetControllingUnknown()
	{
#ifdef _ATL_DEBUG_INTERFACES
		IUnknown* p;
		_AtlDebugInterfacesModule.AddNonAddRefThunk(m_pOuterUnknown, _T("CComEmbeddedObject"), &p);
		return p;
#else
		return m_pOuterUnknown;
#endif
	}
	HRESULT m_hResFinalConstruct;
};

}; //namespace WETLA

#endif // __WETLA_COM_H__
