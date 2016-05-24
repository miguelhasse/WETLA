#ifndef __WETLA_AXUISITE_H__
#define __WETLA_AXUISITE_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCOM_H__
	#error wetla_axuisite.h requires atlcom.h to be included first
#endif

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (push)
#pragma warning(disable: 4702) // unreachable code
#endif //!_ATL_NO_PRAGMA_WARNINGS

namespace WETLA
{

template <class T>
class ATL_NO_VTABLE CAxContainerFrame :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IOleInPlaceFrame
{
public:
	CAxContainerFrame()
	{
	}
	void FinalRelease()
	{
		m_spActiveObject.Release();
	}

	DECLARE_POLY_AGGREGATABLE(CAxContainerFrame<T>)

	BEGIN_COM_MAP(CAxContainerFrame<T>)
		COM_INTERFACE_ENTRY(IOleInPlaceFrame)
		COM_INTERFACE_ENTRY(IOleInPlaceUIWindow)
		COM_INTERFACE_ENTRY(IOleWindow)
	END_COM_MAP()

	DECLARE_EMPTY_MSG_MAP()

// IOleWindow
public:
	STDMETHOD(GetWindow)(HWND* phwnd)
	{
		ATLASSERT(phwnd != NULL);
		if (phwnd == NULL)
			return E_POINTER;

		T* pT = static_cast<T*>(this);
		*phwnd = pT->m_hWnd;
		return S_OK;
	}
	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
	{
		return S_OK;
	}

// IOleInPlaceUIWindow
public:
	STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
	{
		return S_OK;
	}
	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return INPLACE_E_NOTOOLSPACE;
	}
	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return INPLACE_E_NOTOOLSPACE;
	}
	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
	{
		m_spActiveObject = pActiveObject;
		return S_OK;
	}

// IOleInPlaceFrameWindow
public:
	STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS lpMenuWidths)
	{
		ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceFrameWindow::InsertMenus\n"));
		lpMenuWidths->width[0] = lpMenuWidths->width[2] = lpMenuWidths->width[4] = 0;
		return S_OK;
	}
	STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
	{
		ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceFrameWindow::SetMenu\n"));
		return S_OK;
	}
	STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
	{
		ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceFrameWindow::RemoveMenus\n"));
		return S_OK;
	}
	STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
	{
		ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceFrameWindow::SetStatusText\n"));
		return S_OK;
	}
	STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
	{
		ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceFrameWindow::EnableModeless\n"));
		return S_OK;
	}
	STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
	{
		ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceFrameWindow::TranslateAccelerator\n"));
		return S_FALSE;
	}

protected:
	CComPtr<IOleInPlaceActiveObject> m_spActiveObject;
};

template <class T>
class ATL_NO_VTABLE CAxControlContainer :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IOleInPlaceUIWindow,
	public IOleContainer
{
public:
	CAxContainerFrame()
	{
		m_bLocked = FALSE;
	}
	void FinalRelease()
	{
		m_spActiveObject.Release();
	}

	DECLARE_POLY_AGGREGATABLE(CAxControlContainer<T>)

	BEGIN_COM_MAP(CAxControlContainer<T>)
		COM_INTERFACE_ENTRY(IOleWindow)
		COM_INTERFACE_ENTRY(IOleInPlaceUIWindow)
		COM_INTERFACE_ENTRY(IOleContainer)
	END_COM_MAP()

	DECLARE_EMPTY_MSG_MAP()

// IOleWindow
public:
	STDMETHOD(GetWindow)(HWND* phwnd)
	{
		ATLASSERT(phwnd != NULL);
		if (phwnd == NULL)
			return E_POINTER;

		T* pT = static_cast<T*>(this);
		*phwnd = pT->m_hWnd;
		return S_OK;
	}
	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
	{
		return S_OK;
	}

// IOleInPlaceUIWindow
public:
	STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
	{
		return S_OK;
	}
	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return INPLACE_E_NOTOOLSPACE;
	}
	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return INPLACE_E_NOTOOLSPACE;
	}
	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
	{
		m_spActiveObject = pActiveObject;
		return S_OK;
	}
// IOleContainer
public:
	STDMETHOD(ParseDisplayName)(IBindCtx* /*pbc*/, LPOLESTR /*pszDisplayName*/, ULONG* /*pchEaten*/, IMoniker** /*ppmkOut*/)
	{
		ATLTRACENOTIMPL(_T("CAxControlContainer::ParseDisplayName"));
	}
	STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum)
	{
		ATLTRACE(atlTraceHosting, 2, _T("CAxControlContainer::EnumObjects\n"));
		if (ppenum == NULL)
			return E_POINTER;

		*ppenum = NULL;
		if ((grfFlags & OLECONTF_EMBEDDINGS) == 0)
			return S_OK;

		typedef CComObject<CComEnum<IEnumUnknown, &IID_IEnumUnknown,
			IUnknown*, _CopyInterface<IUnknown> > > EnumUnk;

		HRESULT hr;
		EnumUnk* spObjects = NULL;
		if (SUCCEEDED(hr = EnumUnk::CreateInstance(&spObjects)))
		{
			if (SUCCEEDED(hr = spObjects->Init(m_arSites.begin(), m_arSites.end(),
				GetControllingUnknown(), AtlFlagCopy)))
				hr = spObjects->QueryInterface(IID_IEnumUnknown, (LPVOID*)ppenum);
		}
		return hr;
	}
	STDMETHOD(LockContainer)(BOOL fLock)
	{
		m_bLocked = fLock;
		return S_OK;
	}

protected:
	CComDynamicUnkArray m_arSites;
	CComPtr<IOleInPlaceActiveObject> m_spActiveObject;
	unsigned long m_bLocked:1;
};

}; //namespace WETLA

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (pop)
#endif //!_ATL_NO_PRAGMA_WARNINGS

#endif // __WETLA_AXUISITE_H__
