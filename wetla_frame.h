#ifndef __WETLA_FRAME_H__
#define __WETLA_FRAME_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLFRAME_H__
	#error wetla_frame.h requires atlframe.h to be included first
#endif

#if defined(_WIN32_WCE) && !defined(__INCLUDE_SIP__)
	#error wetla_frame.h requires sipapi.h to be included first
#endif

namespace WETLA
{

#ifdef _WIN32_WCE

///////////////////////////////////////////////////////////////////////////////
// CPocketFrameWindowImpl

typedef WTL::CWinTraits<WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0> CPocketFrameTraits;

template <class T, class TBase = ATL::CWindow, class TWinTraits = CPocketFrameTraits>
class ATL_NO_VTABLE CPocketFrameWindowImpl : public WTL::CFrameWindowImpl<T, TBase, TWinTraits>
{
// message map and handlers
public:
	typedef WTL::CFrameWindowImpl<T, TBase, TWinTraits> _baseClass;

	BEGIN_MSG_MAP(CPocketFrameWindowImpl)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		CHAIN_MSG_MAP(_baseClass)
	END_MSG_MAP()

	LRESULT OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	#if defined(_AYGSHELL_H_) || defined(__AYGSHELL_H__)
		::SHDoneButton(m_hWnd, LOWORD(wParam) == WA_INACTIVE ? SHDB_HIDE : SHDB_SHOW);
		::SHHandleWMActivate(m_hWnd, wParam, lParam, &m_sai, 0L);
	#else
		WORD nActive = LOWORD(wParam); 
		if (HIWORD(wParam) == 0 && (nActive == WA_ACTIVE || nActive == WA_CLICKACTIVE))
			OnSettingChange(WM_SETTINGCHANGE, SPI_SETSIPINFO, 0, bHandled);
	#endif
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	#if defined(_AYGSHELL_H_) || defined(__AYGSHELL_H__)
		::SHHandleWMSettingChange(m_hWnd, wParam, lParam, &m_sai);
	#else
		SIPINFO sipi = { sizeof(SIPINFO) };
		if ((wParam == SPI_SETSIPINFO  || wParam == SPI_SETCURRENTIM) && ::SipGetInfo(&sipi))
		{
			if (sipi.fdwFlags & SIPF_DOCKED)
			{
				sipi.rcVisibleDesktop.bottom = (sipi.fdwFlags & SIPF_ON) ? 
					sipi.rcSipRect.top : sipi.rcSipRect.bottom;
			}
			SetWindowPos(NULL, &sipi.rcVisibleDesktop, SWP_FRAMECHANGED | SWP_NOZORDER);
		}
	#endif
		bHandled = FALSE;
		return 0;
	}
};

#endif //_WIN32_WCE

}; //namespace WETLA

#endif // __WETLA_FRAME_H__
