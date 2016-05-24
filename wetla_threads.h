#ifndef __WETLA_THREADS_H__
#define __WETLA_THREADS_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_threads.h requires atlbase.h to be included first
#endif

#ifndef ATL_WORKER_THREAD_WAIT
#define ATL_WORKER_THREAD_WAIT 10000	// time to wait when shutting down
#endif

namespace WETLA
{

/////////////////////////////////////////////////////////////////////////////
// WindowProc thunks

template <class T, class ThreadTraits = DefaultThreadTraits>
class ATL_NO_VTABLE CThreadBase
{
protected:
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	CStdCallThunk thunk;

public:
	CThreadBase() throw() :  m_hThread(NULL),  m_dwThreadId(0)
	{
		typedef DWORD (T::*TMFP)(LPVOID);
		union { DWORD_PTR proc; TMFP func; } procAddrCast;
		procAddrCast.func = T::ThreadProc;
		thunk.Init((DWORD_PTR)procAddrCast.proc, (LPVOID)this);
	}

	~CThreadBase() throw()
	{
		Shutdown();
	}

	operator HANDLE() const throw()
	{
		return m_hThread;
	}

	DWORD GetThreadId() throw()
	{
		return m_dwThreadId;
	}

	HANDLE GetThreadHandle() throw()
	{
		return m_hThread;
	}

	HRESULT Initialize(LPVOID lpParameter = NULL,
		LPSECURITY_ATTRIBUTES lpSecurity = NULL, BOOL bCreateSuspended = FALSE) throw()
	{
		if (m_hThread)
			return E_UNEXPECTED; // already initialized!

		HRESULT hr = NOERROR;
		if ((m_hThread = ThreadTraits::CreateThread(lpSecurity, 0, 
			(LPTHREAD_START_ROUTINE)thunk.GetCodeAddress(), lpParameter, 
			bCreateSuspended ? CREATE_SUSPENDED : 0, &m_dwThreadId)) == NULL)
		{
			hr = AtlHresultFromLastError();
			Shutdown();
		}
		return hr;
	}

	HRESULT Shutdown(DWORD dwWait = ATL_WORKER_THREAD_WAIT) throw()
	{
		if (m_hThread == NULL)
			return S_OK;

		DWORD dwRet = ::WaitForSingleObject(m_hThread, dwWait);
		::CloseHandle(m_hThread);

		return (dwRet == WAIT_OBJECT_0) ? S_OK : AtlHresultFromWin32(dwRet);
	}

	HRESULT Resume() throw()
	{
		return ::ResumeThread(m_hThread) != DWORD(-1) ? S_OK : AtlHresultFromLastError();
	}

	HRESULT Suspend() throw()
	{
		return ::SuspendThread(m_hThread) != DWORD(-1) ? S_OK : AtlHresultFromLastError();
	}

	HRESULT AttachThreadInput(DWORD dwThreadId, BOOL bAttach = TRUE)
	{
		return ::AttachThreadInput(m_hThread, dwThreadId, bAttach) ? S_OK : AtlHresultFromLastError();
	}

	HRESULT GetExitCode(LPDWORD lpExitCode) throw()
	{
		return ::GetExitCodeThread(m_hThread, lpExitCode) ? S_OK : AtlHresultFromLastError();
	}

	HRESULT GetPriority(int* lpPriority) throw()
	{
		if (lpPriority == NULL)
			return E_POINTER;

		HRESULT hr = NOERROR;
		if ((*lpPriority = ::GetThreadPriority(m_hThread)) == THREAD_PRIORITY_ERROR_RETURN)
			hr = AtlHresultFromLastError();

		return hr;
	}

	HRESULT GetPriorityBoost(PBOOL pPriorityBoosting) throw()
	{
		if (pPriorityBoost == NULL)
			return E_POINTER;

		HRESULT hr = NOERROR;
		if (!::GetThreadPriorityBoost(m_hThread, pPriorityBoosting))
			hr = AtlHresultFromLastError();
		else *pPriorityBoosting = ~(*pPriorityBoosting);
		return hr;
	}

	HRESULT SetAffinityMask(DWORD dwMask) throw()
	{
		return ::SetThreadAffinityMask(m_hThread, dwMask) ? S_OK : AtlHresultFromLastError();
	}

#if(_WIN32_WINNT >= 0x0400)
	HRESULT SetIdealProcessor(DWORD dwIdealProcessor) throw()
	{
		return ::SetThreadIdealProcessor(m_hThread, 
			dwIdealProcessor) != DWORD(–1) ? S_OK : AtlHresultFromLastError();
	}
#endif /* _WIN32_WINNT >= 0x0400 */

	HRESULT SetPriority(int nPriority) throw()
	{
		return ::SetThreadPriority(m_hThread, nPriority) ? S_OK : AtlHresultFromLastError();
	}

	HRESULT SetPriorityBoost(BOOL bPriorityBoosting) throw()
	{
		return ::SetThreadPriorityBoost(m_hThread, ~bPriorityBoosting) ? S_OK : AtlHresultFromLastError();
	}

	DWORD ThreadProc(LPVOID lpParameter)
	{
		return 0L;
	}
};

}; //namespace WETLA

#endif // __WETLA_THREADS_H__
