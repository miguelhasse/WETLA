#ifndef __WETLA_CTRLS_H__
#define __WETLA_CTRLS_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_ctrls.h requires atlapp.h to be included first
#endif

#ifndef __ATLWIN_H__
	#error wetla_ctrls.h requires atlwin.h to be included first
#endif

#ifndef __ATLCRACK_H__
	#error wetla_ctrls.h requires atlcrack.h to be included first
#endif

namespace WETLA
{

#ifdef _WIN32_WCE

#include <htmlctrl.h>
#pragma comment(lib, "htmlview.lib")

///////////////////////////////////////////////////////////////////////////////
// CPocketHtmlCtrl

template <class TBase>
class CPocketHtmlCtrlT : public TBase
{
// Constructors
public:
	CPocketHtmlCtrlT(HWND hWnd = NULL) : TBase(hWnd) { }

	CPocketHtmlCtrlT< TBase >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		static BOOL bControlInitialized = FALSE;
		if (!bControlInitialized && !(bControlInitialized = InitHTMLControl(_Module.GetModuleInstance())))
			return NULL;

		return TBase::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
	}

// Attributes
public:
	static LPCTSTR GetWndClassName()
	{
		return WC_HTML; // DISPLAYCLASS
	}

	UINT GetLayoutHeight() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, DTM_LAYOUTHEIGHT, 0, 0L);
	}
	UINT GetLayoutWidth() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, DTM_LAYOUTWIDTH, 0, 0L);
	}
	void EnableClearType(BOOL bEnableClearType)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENABLECLEARTYPE, 0, (LPARAM)bEnableClearType);
	}
	void EnableContextMenu(BOOL bEnableContextMenu)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENABLECONTEXTMENU, 0, (LPARAM)bEnableContextMenu);
	}
	void EnableScripting(BOOL bEnableScripting)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENABLESCRIPTING, 0, (LPARAM)bEnableScripting);
	}
	void EnableShrink(BOOL bEnableShrink)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENABLESHRINK, 0, (LPARAM)bEnableShrink);
	}
	LPDISPATCH GetDocumentDispatch() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LPDISPATCH lpDispatch = NULL;
		::SendMessage(m_hWnd, DTM_DOCUMENTDISPATCH, 0, (LPARAM)&lpDispatch);
		return lpDispatch;
	}
	LPDISPATCH GetScriptDispatch() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LPDISPATCH lpDispatch = NULL;
		::SendMessage(m_hWnd, DTM_SCRIPTDISPATCH, 0, (LPARAM)&lpDispatch);
		return lpDispatch;
	}

// Operations
public:
	void AddText(LPSTR pszText, BOOL bPlainText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ADDTEXT, (WPARAM)bPlainText, (LPARAM)pszText);
	}
	void AddText(LPWSTR pszText, BOOL bPlainText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ADDTEXTW, (WPARAM)bPlainText, (LPARAM)pszText);
	}
	void SetAnchor(LPSTR pszAnchor)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ANCHOR, 0, (LPARAM)pszAnchor);
	}
	void SetAnchor(LPWSTR pszAnchor)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ANCHORW, 0, (LPARAM)pszAnchor);
	}
	void ClearContents()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_CLEAR, 0, 0L);
	}
	void SetEndOfSource()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENDOFSOURCE, 0, 0L);
	}
	void SetImageFail(DWORD dwCookie)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_IMAGEFAIL, 0, (LPARAM)dwCookie);
	}
	void Navigate(LPTSTR pszURL)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_NAVIGATE, 0, (LPARAM)pszURL);
	}
	void SelectAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_SELECTALL, 0, 0L);
	}
	void SetImage(DWORD dwCookie, int iOrigHeight, int iOrigWidth, HBITMAP hbm, BOOL bOwnBitmap)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		INLINEIMAGEINFO i3 = { dwCookie, iOrigHeight, iOrigWidth, hbm, bOwnBitmap };
		::SendMessage(m_hWnd, DTM_IMAGEFAIL, 0, (LPARAM)&i3);
	}
	BOOL SetZoomLevel(DWORD dwZoomLevel)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		BOOL bRetval = (dwZoomLevel >= 0 && dwZoomLevel <= 4);
		if (bRetval) ::SendMessage(m_hWnd, DTM_ZOOMLEVEL, 0, (LPARAM)dwZoomLevel);
		return bRetval;
	}
	void Stop()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_STOP, 0, 0L);
	}
};

typedef CPocketHtmlCtrlT<ATL::CWindow> CPocketHtmlCtrl;

#define HVNM_BASE_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_BASE) \
	{ \
		SetMsgHandled(TRUE); \
		NM_HTMLVIEWA* pHTMLView = (NM_HTMLVIEWA*)lParam; \
		lResult = func(pHTMLView->szTarget, pHTMLView->szData); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_BEFORENAVIGATE_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_BEFORENAVIGATE) \
	{ \
		SetMsgHandled(TRUE); \
		NM_HTMLVIEWA* pHTMLView = (NM_HTMLVIEWA*)lParam; \
		lResult = func(pHTMLView->szTarget, pHTMLView->szData, pHTMLView->dwFlags, pHTMLView->szExInfo); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_CONTEXTMENU_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_CONTEXTMENU) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = func((NM_HTMLCONTEXT*)lParam); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_DOCUMENTCOMPLETE_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_DOCUMENTCOMPLETE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = func(((NM_HTMLVIEWA*)lParam)->szTarget); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_HOTSPOT_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_HOTSPOT) \
	{ \
		SetMsgHandled(TRUE); \
		NM_HTMLVIEWA* pHTMLView = (NM_HTMLVIEWA*)lParam; \
		lResult = func(pHTMLView->szTarget, pHTMLView->szData, pHTMLView->szExInfo); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_INLINE_IMAGE_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_INLINE_IMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		NM_HTMLVIEWA* pHTMLView = (NM_HTMLVIEWA*)lParam; \
		lResult = func(pHTMLView->szTarget, pHTMLView->dwCookie); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_INLINE_SOUND_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_INLINE_SOUND) \
	{ \
		SetMsgHandled(TRUE); \
		NM_HTMLVIEWA* pHTMLView = (NM_HTMLVIEWA*)lParam; \
		lResult = func(pHTMLView->szTarget, pHTMLView->dwCookie); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_INLINE_XML_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_INLINE_XML) \
	{ \
		SetMsgHandled(TRUE); \
		NM_HTMLVIEWA* pHTMLView = (NM_HTMLVIEWA*)lParam; \
		lResult = func(pHTMLView->szTarget, (VARIANT_BOOL)pHTMLView->szData, (IXMLDOMDocument*)pHTMLView->dwCookie); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_METADATA_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_META) \
	{ \
		SetMsgHandled(TRUE); \
		NM_HTMLVIEWA* pHTMLView = (NM_HTMLVIEWA*)lParam; \
		lResult = func(pHTMLView->szTarget, pHTMLView->szData); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_NAVIGATECOMPLETE_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_NAVIGATECOMPLETE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = func(((NM_HTMLVIEWA*)lParam)->szTarget); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_TITLE_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_TITLE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = func(((NM_HTMLVIEWA*)lParam)->szTarget); \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define HVNM_TITLECHANGE_HANDLER(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->code == NM_TITLECHANGE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = func(((NM_HTMLVIEWA*)lParam)->szTarget); \
		if(IsMsgHandled()) \
			return TRUE; \
	}

///////////////////////////////////////////////////////////////////////////////
// CPocketHtmlCtrlImpl - Windows CE HTML View with handlers for notifications

typedef ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | HS_CLEARTYPE> CPocketHtmlCtrlTraits;

template <class T, class TBase = CPocketHtmlCtrl, class TWinTraits = CPocketHtmlCtrlTraits>
class ATL_NO_VTABLE CPocketHtmlCtrlImpl : public ATL::CWindowImpl<T, TBase, TWinTraits>
{
public:
	DECLARE_WND_SUPERCLASS(TEXT("WetlaHtmlCtrl"), TBase::GetWndClassName())

// Constructor/destructor
	CPocketHtmlCtrlImpl() {}
	~CPocketHtmlCtrlImpl() {}

// Attributes
public:

// Operations
public:

// Methods
public:
	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		static BOOL bControlInitialized = FALSE;
		if (!bControlInitialized && !(bControlInitialized = InitHTMLControl(_Module.GetModuleInstance())))
			return NULL;
#if (_MSC_VER >= 1300)
		return ATL::CWindowImpl< T, TBase, TWinTraits >::Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
#else //!(_MSC_VER >= 1300)
		typedef ATL::CWindowImpl< T, TBase, TWinTraits > _baseClass;
		return _baseClass::Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
#endif //!(_MSC_VER >= 1300)
	}
	BOOL LoadFromResource(LPCTSTR lpszResource)
	{
		HINSTANCE hInstance = _Module.GetModuleInstance();
		ASSERT(hInstance != NULL);

		HRSRC hResSrc = FindResource(hInstance, lpszResource, MAKEINTRESOURCE(23));
		if (hResSrc == NULL)
			return FALSE;

		DWORD dwResSize = SizeofResource(hInstance, hResSrc);
		HGLOBAL hResource = LoadResource(hInstance, hResSrc);
		if (hResource == NULL)
			return FALSE;

		BYTE* pResData = (BYTE*)LockResource(hResource);
		AddText((LPSTR)pResData, FALSE);
		SetEndOfSource();

		UnlockResource(hResource);
		FreeResource(hResource);
		return TRUE;
	}
	BOOL LoadFromResource(UINT nRes)
	{
		return LoadFromResource(MAKEINTRESOURCE(nRes));
	}

// Message map and handlers
public:
	BEGIN_MSG_MAP_EX(CPocketHtmlCtrlImpl)
		HVNM_BASE_HANDLER(OnBase)
		HVNM_BEFORENAVIGATE_HANDLER(OnBeforeNavigate)
		HVNM_CONTEXTMENU_HANDLER(OnContextMenu)
		HVNM_DOCUMENTCOMPLETE_HANDLER(OnDocumentComplete)
		HVNM_HOTSPOT_HANDLER(OnHotSpot)
		HVNM_INLINE_IMAGE_HANDLER(OnInlineImage)
		HVNM_INLINE_SOUND_HANDLER(OnInlineSound)
		HVNM_INLINE_XML_HANDLER(OnInlineXML)
		HVNM_METADATA_HANDLER(OnMetadata)
		HVNM_NAVIGATECOMPLETE_HANDLER(OnNavigateComplete)
		HVNM_TITLE_HANDLER(OnTitle)
		HVNM_TITLECHANGE_HANDLER(OnTitleChange)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnBase(LPCSTR pszHREF, LPCSTR pszTARGET)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnBeforeNavigate(LPCSTR pszURL, LPCSTR pszPostData, DWORD dwFlags, LPCSTR pszHeaders)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnContextMenu(NM_HTMLCONTEXT* pHTMLContext)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnDocumentComplete(LPCSTR pszURL)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnHotSpot(LPCSTR pszHREF, LPCSTR pszPostData, LPCSTR pszTARGET)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnInlineImage(LPCSTR pszSRC, DWORD dwCookieValue)
	{
		// Use undocumented function SHLoadImageFile (returns HBITMAP)
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnInlineSound(LPCSTR pszSRC, DWORD dwLoopCount)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnInlineXML(LPCSTR pszSRC, VARIANT_BOOL vbASYNC, IXMLDOMDocument* pXMLDocument)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnMetadata(LPCSTR pszHTTPEQUIV, LPCSTR pszCONTENT)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnNavigateComplete(LPCSTR pszURL)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnTitle(LPCSTR pszTitle)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
	LRESULT OnTitleChange(LPCSTR pszTitle)
	{
		T* pT = static_cast<T*>(this);
		return 0;
	}
};

#endif //_WIN32_WCE

}; //namespace WETLA

#endif // __WETLA_CTRLS_H__
