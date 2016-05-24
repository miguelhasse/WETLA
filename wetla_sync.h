#ifndef __WETLA_SYNC_H__
#define __WETLA_SYNC_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_sync.h requires atlbase.h to be included first
#endif

#pragma once

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(push)
#pragma warning(disable: 4512)  // assignment operator could not be generated
#endif  // !_ATL_NO_PRAGMA_WARNINGS

namespace WETLA
{

#define MAXIMUM_WAIT_TIMEOUT 2000

class CReadWriteSection
{
public:
	CReadWriteSection(LPSECURITY_ATTRIBUTES lpSecurity = NULL);
	~CReadWriteSection() throw();

	BOOL ReaderLock(DWORD dwTimeOut = MAXIMUM_WAIT_TIMEOUT) throw;
	void ReaderUnlock() throw;
	BOOL WriterLock(DWORD dwTimeOut = MAXIMUM_WAIT_TIMEOUT) throw;
	void WriterUnlock() throw;

	BOOL CanRead() throw;
	BOOL CanWrite() throw;

// Implementation
private:
    HANDLE m_hWritingMutex; // single reader access to the reader counter
    HANDLE m_hDataLock; // data lock for readers and writers
	volatile int m_nReaderCount; // count of the number of readers
};

inline CReadWriteSection::CReadWriteSection(LPSECURITY_ATTRIBUTES lpSecurity) : 
	m_nReaderCount(0), m_hWritingMutex(NULL)
{
	if ((m_hDataLock = ::CreateSemaphore(lpSecurity, 1, 1, NULL)) == NULL)
		AtlThrowLastWin32();

	if ((m_hWritingMutex = ::CreateMutex(lpSecurity, FALSE, NULL)) == NULL)
	{
		::CloseHandle(m_hDataLock);
		m_hDataLock = NULL;
		AtlThrowLastWin32();
	}
}

inline CReadWriteSection::~CReadWriteSection() throw()
{
	if (::WaitForSingleObject(m_hDataLock, 0) == WAIT_TIMEOUT)
		ATLTRACE(atlTraceSync, 0, _T("FatalError: Can't destroy object, it's locked!\n"));

	::CloseHandle(m_hWritingMutex);
	::CloseHandle(m_hDataLock);
}

BOOL CReadWriteSection::ReaderLock(DWORD dwTimeOut) throw
{
	if (::WaitForSingleObject(m_hWritingMutex, MAXIMUM_WAIT_TIMEOUT) != WAIT_OBJECT_0)
		return FALSE;

	BOOL bSuccess = TRUE;
	if (++m_nReaderCount == 1)
		bSuccess = ::WaitForSingleObject(m_hDataLock, MAXIMUM_WAIT_TIMEOUT) == WAIT_OBJECT_0;

	::ReleaseMutex(m_hWritingMutex);
	return bSuccess;
}

BOOL CReadWriteSection::ReaderUnlock() throw
{
	if (::WaitForSingleObject(m_hWritingMutex, MAXIMUM_WAIT_TIMEOUT) != WAIT_OBJECT_0)
		return FALSE;

	BOOL bSuccess = TRUE;
	if (--m_nReaderCount == 0)
		bSuccess = ::ReleaseSemaphore(m_hDataLock, 1, NULL);

	::ReleaseMutex(m_hWritingMutex);
	return bSuccess;
}

BOOL CReadWriteSection::WriterLock(DWORD dwTimeOut) throw
{
	return (::WaitForSingleObject(m_hDataLock, dwTimeOut) == WAIT_OBJECT_0);
}

BOOL CReadWriteSection::WriterUnlock() throw
{
	return ::ReleaseSemaphore(m_hDataLock, 1, NULL);
}

inline BOOL CReadWriteSection::CanRead() throw
{
	return m_nReaderCount > 0;
}

BOOL CReadWriteSection::CanWrite() throw
{
    // The first reader may be waiting in the mutex,
    // but any more than that is an error.
    if (m_nReaderCount > 1)
        return FALSE;

    if (::WaitForSingleObject(m_hDataLock, 0) == WAIT_TIMEOUT)
        return TRUE;

    // A count is kept, which was incremented in Wait...
    if (::ReleaseSemaphore(m_hDataLock, 1, NULL) == FALSE)
		ATLTRACE(atlTraceSync, 0, _T("FatalError: ReleaseSemaphore failed!\n"));

    return FALSE;
}

}; //namespace WETLA

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(pop)
#endif  // !_ATL_NO_PRAGMA_WARNINGS

#endif // __WETLA_SYNC_H__
