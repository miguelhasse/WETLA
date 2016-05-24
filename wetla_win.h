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

struct _WETLA_WNDCLASSINFO
{
	WNDCLASSEX m_wc;
	LPCTSTR m_lpszOrigName;
	WNDPROC m_pWndProc;
	LPCTSTR m_lpszCursorID;
	BOOL m_bSystemCursor;
	ATOM m_atom;

	ATOM Register()
	{
		if (m_atom == 0)
		{
			ATL::CComCritSecLock<ATL::CComCriticalSection> lock(_AtlWinModule.m_csWindowCreate, false);
			if (FAILED(lock.Lock()))
			{
				ATLTRACE(atlTraceWindowing, 0, _T("ERROR : Unable to lock critical section in _WETLA_WNDCLASSINFO.Register\n"));
				ATLASSERT(0);
				return 0;
			}
			if (m_atom == 0)
			{
				if (m_lpszOrigName != NULL)
				{
					LPCTSTR lpsz = m_wc.lpszClassName;
					WNDPROC proc = m_wc.lpfnWndProc;
					WNDCLASSEX wc = { sizeof(WNDCLASSEX) };

					// Try global class
					if (!::GetClassInfoEx(NULL, m_lpszOrigName, &wc))
					{
						// try process local
						if (!::GetClassInfoEx(_AtlBaseModule.GetModuleInstance(), m_lpszOrigName, &wc))
						{
							ATLTRACE(atlTraceWindowing, 0, "ERROR : Could not obtain Window Class information for %s\n", m_lpszOrigName);
							return 0;
						}
					}
					m_wc = wc;
					m_pWndProc = m_wc.lpfnWndProc;
					m_wc.lpszClassName = lpsz;
					m_wc.lpfnWndProc = proc;
				}
				else
				{
					m_wc.hCursor = ::LoadCursor(
						m_bSystemCursor ? NULL : _AtlBaseModule.GetResourceInstance(),
						m_lpszCursorID);

					if (m_pWndProc == NULL)
						m_pWndProc = ::DefWindowProc;
				}

				m_wc.hInstance = _AtlBaseModule.GetModuleInstance();
				m_wc.style |= CS_GLOBALCLASS;	// always register as global classes

				if (m_wc.lpszClassName == NULL)
				{
					ATLTRACE(atlTraceWindowing, 0, _T("ERROR : No class name defined for the window control\n"));
					ATLASSERT(0);
					return 0;
				}
				WNDCLASSEX wcTemp = m_wc;
				if ((m_atom = static_cast<ATOM>(::GetClassInfoEx(
					m_wc.hInstance, m_wc.lpszClassName, &wcTemp))) == 0)
				{
					m_atom = ATL::AtlWinModuleRegisterClassEx(&_AtlWinModule, &m_wc);
				}
			}
		}
		return m_atom;
	}
};

typedef _WETLA_WNDCLASSINFO CControlClassInfo;


/////////////////////////////////////////////////////////////////////////////
// CControlClassInfo - Manages Windows global control class information

#define DECLARE_CTRL_CLASS(WndClassName) \
static WETLA::CControlClassInfo& GetControlClassInfo() \
{ \
	static WETLA::CControlClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0 \
	}; \
	return wc; \
}

#define DECLARE_CTRL_CLASS_EX(WndClassName, style, bkgnd) \
static WETLA::CControlClassInfo& GetControlClassInfo() \
{ \
	static WETLA::CControlClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), style, WindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(bkgnd + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0 \
	}; \
	return wc; \
}

#define DECLARE_CTRL_SUPERCLASS(WndClassName, OrigWndClassName) \
static WETLA::CControlClassInfo& GetControlClassInfo() \
{ \
	static WETLA::CControlClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), 0, WindowProc, \
		  0, 0, NULL, NULL, NULL, NULL, NULL, WndClassName, NULL }, \
		OrigWndClassName, NULL, NULL, TRUE, 0 \
	}; \
	return wc; \
}

/////////////////////////////////////////////////////////////////////////////
// CWindowControlImpl - Implements a global window control

template <class T, class TBase = ATL::CWindow>
class ATL_NO_VTABLE CWindowControlImpl : public ATL::CWindowImplRoot<TBase>
{
public:
	WNDPROC m_pfnSuperWindowProc;

	CWindowControlImpl(HWND hWnd) : m_pfnSuperWindowProc(T::GetControlClassInfo().m_pWndProc)
	{
		m_hWnd = hWnd;
	}

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static CControlClassInfo& GetControlClassInfo()
	{
		if (TBase::GetWndClassName() == NULL)
		{
			ATLTRACE(atlTraceWindowing, 0, _T("ERROR : No class name defined for the window control\n"));
			ATLASSERT(0);
		}
		static CControlClassInfo wc =
		{
			{ sizeof(WNDCLASSEX), CS_GLOBALCLASS, StartWindowProc,
			  0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW + 1), 0, TBase::GetWndClassName(), 0 },
			NULL, NULL, IDC_ARROW, TRUE, 0
		};
		return wc;
	}

	BOOL DestroyWindow()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::DestroyWindow(m_hWnd);
	}

	LRESULT NotifyParent(UINT uiCode)
	{
		NMHDR nmh = { m_hWnd, GetWindowLong(GWL_ID), uiCode };
		return (LRESULT)::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nmh.idFrom, (LPARAM)&nmh);
	}

	LRESULT DefWindowProc()
	{
		const _ATL_MSG* pMsg = m_pCurrentMsg;
		return (pMsg != NULL) ? DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam) : 0;
	}
	LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
#ifdef STRICT
		return ::CallWindowProc(m_pfnSuperWindowProc, m_hWnd, uMsg, wParam, lParam);
#else
		return ::CallWindowProc((FARPROC)m_pfnSuperWindowProc, m_hWnd, uMsg, wParam, lParam);
#endif
	}

	virtual WNDPROC GetWindowProc()
	{
		return WindowProc;
	}
	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// override to do something, if needed
	}
};

template <class T, class TBase>
LRESULT CALLBACK CWindowControlImpl<T, TBase>::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Find C++ object associated with the window
	T* pThis = reinterpret_cast<T*>(::GetWindowLong(hWnd, GWL_USERDATA));

	if (pThis == 0) // the class hasn't been created yet
	{
		pThis = new T(hWnd); // create a new control (a default constructor must be implemented)
		ATLASSERT(pThis != NULL);
		::SetLastError(0L); // clear the last error information
		::SetWindowLongPtr(hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		ATLASSERT(GetLastError() == 0L);
	}

	// set a ptr to this message and save the old value
	_ATL_MSG msg(pThis->m_hWnd, uMsg, wParam, lParam);
	const _ATL_MSG* pOldMsg = pThis->m_pCurrentMsg;
	pThis->m_pCurrentMsg = &msg;

	// pass to the message map to process
	LRESULT lRes;
	BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);

	// restore saved value for the current message
	ATLASSERT(pThis->m_pCurrentMsg == &msg);
	pThis->m_pCurrentMsg = pOldMsg;

	// do the default processing if message was not handled
	if (!bRet)
	{
		if (uMsg == WM_NCDESTROY) //WCE_IF(WM_DESTROY, WM_NCDESTROY))
		{
			// unsubclass, if needed
			LONG_PTR pfnWndProc = ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC);
			lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
			if (pThis->m_pfnSuperWindowProc != ::DefWindowProc &&
				::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC) == pfnWndProc)
				::SetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC, (LONG_PTR)pThis->m_pfnSuperWindowProc);
			// mark window as destryed
			pThis->m_dwState |= WINSTATE_DESTROYED;
		}
		else (lRes = pThis->DefWindowProc(uMsg, wParam, lParam));
	}
	if ((pThis->m_dwState & WINSTATE_DESTROYED) && pThis->m_pCurrentMsg == NULL)
	{
		// clear out window handle
		HWND hWnd = pThis->m_hWnd;
		pThis->m_hWnd = NULL;
		pThis->m_dwState &= ~WINSTATE_DESTROYED;
		// clean up after window is destroyed
		pThis->OnFinalMessage(hWnd);
	        delete pThis;
	}
	return lRes;
}

}; //namespace WETLA

#endif // __WETLA_WIN_H__
