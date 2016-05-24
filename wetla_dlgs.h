#ifndef __WETLA_DLGS_H__
#define __WETLA_DLGS_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_dlgs.h requires atlapp.h to be included first
#endif

#ifndef __ATLWIN_H__
	#error wetla_dlgs.h requires atlwin.h to be included first
#endif

#ifndef __ATLDLGS_H__
	#error wetla_dlgs.h requires atldlgs.h to be included first
#endif

namespace WETLA
{

template <class T, class TBase = CPropertySheetWindow>
class ATL_NO_VTABLE CPropertySheetImpl : public WTL::CPropertySheetImpl<T, TBase>
{
// Construction/Destruction
public:
	CPropertySheetImpl(ATL::_U_STRINGorID title = (LPCTSTR)NULL, UINT uStartPage = 0, HWND hWndParent = NULL)
	{
		memset(&m_psh, 0, sizeof(PROPSHEETHEADER));
		m_psh.dwSize = sizeof(PROPSHEETHEADER);
#ifdef WIN32_PLATFORM_PSPC
		m_psh.dwFlags = PSH_USECALLBACK | PSH_MAXIMIZE ;
#else //!defined(WIN32_PLATFORM_PSPC)
		m_psh.dwFlags = PSH_USECALLBACK;
#endif	//WIN32_PLATFORM_PSPC
#if (_ATL_VER >= 0x0700)
		m_psh.hInstance = ATL::_AtlBaseModule.GetResourceInstance();
#else //!(_ATL_VER >= 0x0700)
		m_psh.hInstance = _Module.GetResourceInstance();
#endif //(_ATL_VER >= 0x0700)
		m_psh.phpage = NULL;   // will be set later
		m_psh.nPages = 0;      // will be set later
		m_psh.pszCaption = title.m_lpstr;
		m_psh.nStartPage = uStartPage;
		m_psh.hwndParent = hWndParent;   // if NULL, will be set in DoModal/Create
		m_psh.pfnCallback = T::PropSheetCallback;
	}

	static int CALLBACK PropSheetCallback(HWND hWnd, UINT uMsg, LPARAM)
	{
		if (uMsg == PSCB_INITIALIZED)
		{
			ATLASSERT(hWnd != NULL);
#if (_ATL_VER >= 0x0700)
			T* pT = (T*)ATL::_AtlWinModule.ExtractCreateWndData();
#else //!(_ATL_VER >= 0x0700)
			T* pT = (T*)_Module.ExtractCreateWndData();
#endif //(_ATL_VER >= 0x0700)
			// subclass the sheet window
			pT->SubclassWindow(hWnd);
			// remove page handles array
			pT->_CleanUpPages();
		}
#ifdef WIN32_PLATFORM_PSPC
		if (uMsg == PSCB_GETVERSION)
			return COMCTL32_VERSION;
#endif //WIN32_PLATFORM_PSPC
		return 0;
	}

// Message map and handlers
public:
	BEGIN_MSG_MAP(CPropertySheetImpl)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
	END_MSG_MAP()

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
		if (HIWORD(wParam) == BN_CLICKED && 
			((LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) ||
		#if (_WIN32_IE >= 0x0500) && defined(PSH_WIZARD_LITE)
			((m_psh.dwFlags & (PSH_WIZARD | PSH_WIZARD97 | PSH_WIZARD_LITE)) != 0)) &&
		#elif (_WIN32_IE >= 0x0400) && defined(PSH_WIZARD97)
			((m_psh.dwFlags & (PSH_WIZARD | PSH_WIZARD97)) != 0)) &&
		#else
			((m_psh.dwFlags & PSH_WIZARD) != 0)) &&
		#endif
			((m_psh.dwFlags & PSH_MODELESS) != 0) && (GetActivePage() == NULL))
			DestroyWindow();
		return lRet;
	}
};

}; //namespace WETLA

#endif // __WETLA_DLGS_H__
