#ifndef __WETLA_WINHOOKS_H__
#define __WETLA_WINHOOKS_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_winhooks.h requires atlbase.h to be included first
#endif

namespace WETLA
{

/////////////////////////////////////////////////////////////////////////////
// WindowProc thunks

template <class T, int HookType>
class ATL_NO_VTABLE CWindowsHookBase
{
protected:
	HHOOK m_hHook;
	CStdCallThunk thunk;

public:
	CWindowsHookBase() throw() :  m_hHook(NULL)
	{
		typedef DWORD (T::*TMFP)(LPVOID);
		union { DWORD_PTR proc; TMFP func; } procAddrCast;
		procAddrCast.func = T::HookProc;
		thunk.Init((DWORD_PTR)procAddrCast.proc, (LPVOID)this);
	}

	~CWindowsHookBase() throw()
	{
		RemoveHook();
	}

	operator HANDLE() const throw()
	{
		return m_hHook;
	}

	BOOL SetHook(DWORD dwThreadId) throw()
	{
		if (0xC086 & (WORD(1) << (HookType + 1)))
			dwThreadId = 0L;

		m_hHook = ::SetWindowsHookEx(HookType,
			(HOOKPROC)thunk.GetCodeAddress(), NULL, dwThreadId);
		return (m_hHook != NULL);
	}

	BOOL RemoveHook() throw()
	{
		return m_hHook ? UnhookWindowsHookEx(m_hHook) : TRUE;
	}

	LRESULT HookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		return (nCode < 0) ? ::CallNextHookEx(m_hHook, nCode, wParam, lParam) : 0L;
	}
};

class ATL_NO_VTABLE CCallWndprocHook : public CWindowsHookBase<CCallWndprocHook, WH_CALLWNDPROC>
{
	LRESULT HookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		switch(nCode)
		{
		case HC_ACTION:
			{
				BOOL bFromCurrentThread = (wParam != 0);
				PCWPSTRUCT lpCWP = reinterpret_cast<PCWPSTRUCT>(lParam);
			}
			break;

		default:
			return 0L;
		}
		return __super::HookProc(nCode, wParam, lParam);
	}
};

class ATL_NO_VTABLE CMouseProcHook : public CWindowsHookBase<CMouseProcHook, WH_MOUSE>
{
	LRESULT HookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		switch(nCode)
		{
		case HC_ACTION:
			{
				UINT message = static_cast<UINT>(wParam);
				PMOUSEHOOKSTRUCT lpCWP = reinterpret_cast<PMOUSEHOOKSTRUCT>(lParam);
			}
			break;

		case HC_NOREMOVE:
			{
				UINT message = static_cast<UINT>(wParam);
				PMOUSEHOOKSTRUCT lpCWP = reinterpret_cast<PMOUSEHOOKSTRUCT>(lParam);
			}
			break;

		default:
			return 0L;
		}
		return __super::HookProc(nCode, wParam, lParam);
	}
};

}; //namespace WETLA

#endif // __WETLA_WINHOOKS_H__
