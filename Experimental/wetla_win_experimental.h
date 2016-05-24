#ifndef __WETLA_WIN_H__
#define __WETLA_WIN_H__

// Implementation Helpers for Windows Controls

#ifndef __cplusplus
	#error wetla_win.h requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLWIN_H__
	#error wetla_win.h requires atlwin.h to be included first
#endif

namespace WETLA
{

#define DECLARE_CTRL_WND_CLASS(WndClassName, style, bkgnd) \
static CWndClassInfo& GetWndClassInfo() \
{ \
	static CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), style | CS_GLOBALCLASS, WindowProc, \
			0, 0, NULL, NULL, NULL, (HBRUSH)(bkgnd + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("") \
	}; \
	return wc; \
}

#define DECLARE_CTRL_WND_SUPERCLASS(WndClassName, OrigClassName) \
static CWndClassInfo& GetWndClassInfo() \
{ \
	static CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_GLOBALCLASS, WindowProc, \
			0, 0, NULL, NULL, NULL, (HBRUSH) NULL, NULL, WndClassName, NULL }, \
		OrigClassName, NULL, IDC_ARROW, TRUE, 0, _T("") \
	}; \
	return wc; \
}

template <class T, class TBase = CWindow>
class ATL_NO_VTABLE CWindowControlImpl : public TBase, public CMessageMap
{
// Construction / destruction
public:
	CWindowControlImpl(HWND hWnd) : TBase(hWnd), m_pCurrentMsg(NULL)
	{
	}

// Current message
	const _ATL_MSG* GetCurrentMessage() const
	{
		return m_pCurrentMsg;
	}

// Handled management for cracked handlers
	BOOL IsMsgHandled() const
	{
		const _ATL_MSG* pMsg = GetCurrentMessage();
		ATLASSERT(pMsg != NULL);
		ATLASSERT(pMsg->cbSize >= sizeof(_ATL_MSG));
		return pMsg->bHandled;
	}

	void SetMsgHandled(BOOL bHandled)
	{
		_ATL_MSG* pMsg = (_ATL_MSG*)GetCurrentMessage();	// override const
		ATLASSERT(pMsg != NULL);
		ATLASSERT(pMsg->cbSize >= sizeof(_ATL_MSG));
		pMsg->bHandled = bHandled;
	}

// Registration Helpers
public:
	static ATOM RegisterClass(HINSTANCE hInstance);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Window Procedure
protected:
	LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		_ATL_MSG msg(m_hWnd, uMsg, wParam, lParam);
		const _ATL_MSG* pOldMsg = m_pCurrentMsg;
		m_pCurrentMsg = &msg;
		LRESULT nResult = 0L;

		T* pT = static_cast<T*>(this);
		if (!pT->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, nResult))
			nResult = ::CallWindowProc(pT->GetWndClassInfo().pWndProc,
				m_hWnd, uMsg, wParam, lParam);

		// restore saved value for the current message
		ATLASSERT(m_pCurrentMsg == &msg);
		m_pCurrentMsg = pOldMsg;
		return nResult;
	}

// Helpers
protected:
	void NotifyParent(UINT code)
	{
		NMHDR nmh = {m_hWnd, GetWindowLong(GWL_ID), code};
		::SendMessage(GetParent(), WM_NOTIFY, (WPARAM) nmh.idFrom, (LPARAM) &nmh);
	}

	BOOL InvalidateRect(BOOL bErase = TRUE)
	{
		CRect rect;
		GetClientRect(&rect);
		return CWindow::InvalidateRect(rect, bErase);
	}

// Implementation data
protected:
	const _ATL_MSG* m_pCurrentMsg;
};

template <class T, class TBase>
ATOM CWindowControlImpl<T, TBase>::RegisterClass(HINSTANCE hInstance)
{
	CWndClassInfo& ci = T::GetWndClassInfo();

	if (ci.m_atom == 0)
	{
		ATL::CComCritSecLock<ATL::CComCriticalSection> lock(_AtlWinModule.m_csWindowCreate, false);
		if (FAILED(lock.Lock()))
		{
			ATLTRACE(atlTraceWindowing, 0, _T("ERROR : Unable to lock critical section in RegisterClass\n"));
			ATLASSERT(0);
			return 0;
		}
		if (ci.m_atom == 0)
		{
			if (ci.m_lpszOrigName != 0) // Windows SuperClassing
			{
				LPCTSTR lpsz = ci.m_wc.lpszClassName;
				WNDPROC proc = ci.m_wc.lpfnWndProc;

				WNDCLASSEX wndClass;
				wndClass.cbSize = sizeof(WNDCLASSEX);

				// Try global class
				if (!::GetClassInfoEx(NULL, ci.m_lpszOrigName, &wndClass))
				{
					// Try local class
					if (!::GetClassInfoEx(_AtlBaseModule.GetModuleInstance(),
						ci.m_lpszOrigName, &wndClass))
						return 0;
				}

				CopyMemory(&ci.m_wc, &wndClass, sizeof(WNDCLASSEX));
				ci.pWndProc = ci.m_wc.lpfnWndProc;
				ci.m_wc.lpszClassName = lpsz;
				ci.m_wc.lpfnWndProc = proc;

			} 
			else // Traditionnal registration
			{
				ci.m_wc.hCursor = ::LoadCursor(ci.m_bSystemCursor ? 
					NULL : _AtlBaseModule.GetModuleInstance(), ci.m_lpszCursorID);
				ci.pWndProc = ::DefWindowProc;
			}

			ci.m_wc.hInstance = _AtlBaseModule.GetModuleInstance();
			ci.m_wc.style |= CS_GLOBALCLASS;

			// Synthetize custom class name
			if (ci.m_wc.lpszClassName == 0) 
			{ 
				wsprintf(ci.m_szAutoName, _T("WTL:%8.8X"), (DWORD) &ci.m_wc);
				ci.m_wc.lpszClassName = ci.m_szAutoName;
			}

			// Check previous registration...
			WNDCLASSEX wndClassTemp;
			CopyMemory(&wndClassTemp, &ci.m_wc, sizeof(WNDCLASSEX));
			if ((ci.m_atom = (ATOM)::GetClassInfoEx(ci.m_wc.hInstance,
				ci.m_wc.lpszClassName, &wndClassTemp)) == 0)
				ci.m_atom = ::RegisterClassEx(&ci.m_wc);
		}
	}
	return ci.m_atom;
}

template <class T, class TBase>
LRESULT CALLBACK CWindowControlImpl<T, TBase>::WindowProc(HWND hWnd,
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Find C++ object associated with the window
	T* pT = reinterpret_cast<T*>(::GetWindowLong(hWnd, GWL_USERDATA));

	// Note: Windows can send a WM_GETMINMAXINFO message to a window prior to
	// sending a WM_NCCREATE or WM_CREATE message.

	if (pT == 0) // The class hasn't been created yet
	{
		if (uMsg == WCE_IF(WM_CREATE, WM_NCCREATE)
		{
			pT = new T(hWnd);
			::SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(pT));
			return pT->WindowProc(uMsg, wParam, lParam);
		}
		else return ::CallWindowProc(T::GetWndClassInfo().pWndProc,
			hWnd, uMsg, wParam, lParam);
	} 
	else
	{
		if (uMsg == WCE_IF(WM_DESTROY, WM_NCDESTROY))
		{
			::SetWindowLong(hWnd, GWL_USERDATA, 0L);
			delete pT;
			return ::CallWindowProc(T::GetWndClassInfo().pWndProc,
				hWnd, uMsg, wParam, lParam);
		}
		else return pT->WindowProc(uMsg, wParam, lParam);
	}

	return 0L;
}

}; //namespace WETLA

#endif // __WETLA_WIN_H__
