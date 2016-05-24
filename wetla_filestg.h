#ifndef __WETLA_FILESTG_H__
#define __WETLA_FILESTG_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_filestg.h requires atlapp.h to be included first
#endif

#include <winioctl.h>

namespace WETLA
{

// MessageId: E_INVALID_FUNCTION
// MessageText: Incorrect function
#define E_INVALID_FUNCTION _HRESULT_TYPEDEF_(0x80070001L)

// MessageId: E_INVALID_PARAMETER
// MessageText: The parameter is incorrect
#define E_INVALID_PARAMETER _HRESULT_TYPEDEF_(0x80070057L)

// MessageId: E_OPERATION_ABORTED
// MessageText: The I/O operation has been aborted because 
//              of either a thread exit or an application request
#define E_OPERATION_ABORTED _HRESULT_TYPEDEF_(0x800703E3L)

// MessageId: E_IO_INCOMPLETE
// MessageText: Overlapped I/O event is not in a signaled state
#define E_IO_INCOMPLETE _HRESULT_TYPEDEF_(0x800703E4L)

// MessageId: E_IO_PENDING
// MessageText: Overlapped I/O operation is in progress
#define E_IO_PENDING _HRESULT_TYPEDEF_(0x800703E5L)

// MessageId: E_JOURNAL_DELETE_IN_PROGRESS
// MessageText: The volume change journal is being deleted
#define E_JOURNAL_DELETE_IN_PROGRESS _HRESULT_TYPEDEF_(0x8007049AL)

// MessageId: E_JOURNAL_NOT_ACTIVE
// MessageText: The volume change journal is not active
#define E_JOURNAL_NOT_ACTIVE _HRESULT_TYPEDEF_(0x8007049BL)

// MessageId: E_JOURNAL_ENTRY_DELETED
// MessageText: The journal entry has been deleted from the journal
#define E_JOURNAL_ENTRY_DELETED _HRESULT_TYPEDEF_(0x8007049DL)


template<bool TAsync = false>
class CDeviceControlBase : public CHandle
{
private:
	OVERLAPPED m_oli; // overlapped structure for async control with completion packets
	OVERLAPPED m_ols; // overlapped structure for async control with event signaling
	bool m_beolevent;
	HANDLE m_olQueue;

public:
	CDeviceControlBase(HANDLE hSignalEvent = NULL) : 
		m_beolevent(false), m_olQueue(NULL)
	{
		ATLASSERT(m_h == NULL);

		if (TAsync)
		{
			ZeroMemory(&m_oli, sizeof(OVERLAPPED));
			ZeroMemory(&m_ols, sizeof(OVERLAPPED));
			m_beolevent = (hSignalEvent != NULL);
			m_ols.hEvent = m_beolevent ? hSignalEvent :
				CreateEvent(NULL, FALSE, FALSE, NULL);
		}
	}

	virtual ~CDeviceControlBase()
	{
		if (m_ols.hEvent && !m_beolevent)
			CloseHandle(m_ols.hEvent);
	}

	HANDLE GetEventHandle() { return m_ols.hEvent; }

public:
	HRESULT Initialize(LPCTSTR szDevice, DWORD dwDesiredAccess,
		DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpsa = NULL, 
		DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL) throw()
	{
		ATLASSERT(m_h == NULL);
		HANDLE hDevice = ::CreateFile(szDevice,
			dwDesiredAccess, dwShareMode, lpsa, OPEN_EXISTING,
			(TAsync ? FILE_FLAG_OVERLAPPED : 0) | dwFlagsAndAttributes, NULL);

		if (hDevice == INVALID_HANDLE_VALUE)
			return AtlHresultFromLastError();

		Attach(hDevice);
		return S_OK;
	}

	HRESULT DeviceIoControl(DWORD dwIoControlCode, 
		LPVOID lpInBuffer, DWORD nInBufferSize,
		LPVOID lpOutBuffer, DWORD nOutBufferSize,
		LPDWORD lpBytesReturned, bool bSendCompletion = true) throw()
	{
		BOOL bRetVal = ::DeviceIoControl(m_h, dwIoControlCode,
			lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize,
			lpBytesReturned, (bSendCompletion && m_olQueue) ? &m_oli : &m_ols);

		return (!bRetVal) ? AtlHresultFromLastError() : S_OK;
	}

	HRESULT GetOverlappedResult(DWORD& dwBytesTransferred, BOOL bWait) throw()
	{
		if (!::GetOverlappedResult(m_h, &m_ols, &dwBytesTransferred, bWait))
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT CancelIoOperations() throw()
	{
		if (!::CancelIo(m_h))
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT CreateCompletionPort(HANDLE hExistingPort = INVALID_HANDLE_VALUE,
		ULONG_PTR pCompletionKey = NULL) throw()
	{
		ATLASSERT(m_olQueue == NULL);
		HANDLE hPort = NULL;
		if (hPort = CreateIoCompletionPort(m_h, hExistingPort, pCompletionKey, 0L))
		{
			m_olQueue = hPort;
			m_ols.hEvent = (HANDLE)((DWORD)m_ols.hEvent | 0x1);
		}
		return ((hExistingPort == INVALID_HANDLE_VALUE && hPort == NULL) ||
			hExistingPort != hPort) ? AtlHresultFromLastError() : S_OK;
	}

	HRESULT GetCompletionStatus(DWORD& dwNumberOfBytes,
		ULONG_PTR& pCompletionKey, DWORD dwMilliseconds = INFINITE) throw()
	{
		ATLASSERT(m_olQueue != NULL);
		LPOVERLAPPED lpOverlapped = NULL;
		return !GetQueuedCompletionStatus(m_olQueue, &dwNumberOfBytes, 
			&pCompletionKey, &lpOverlapped, dwMilliseconds) ?
			AtlHresultFromLastError() : S_OK;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CChangeJornal

template<bool TAsync = false, DWORD TPageBuffers = 4>
class CChangeJornal : public CDeviceControlBase<TAsync>
{
public:
	CChangeJornal(HANDLE hSignalEvent = NULL) : 
		CDeviceControlBase<TAsync>(hSignalEvent), 
		m_nDataLength(0), m_pUsnRecord(NULL)
	{
		ZeroMemory(&m_rujd, sizeof(READ_USN_JOURNAL_DATA));
		ZeroMemory(&m_ujd, sizeof(USN_JOURNAL_DATA));

		// Allocate internal buffer...
		m_pDataBuffer = (PBYTE)HeapAlloc(GetProcessHeap(), 
			HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
			TPageBuffers * USN_PAGE_SIZE + sizeof(USN));
	}

	virtual ~CChangeJornal()
	{
		if (m_pDataBuffer)
			HeapFree(GetProcessHeap(), 0, m_pDataBuffer);
	}

	HRESULT Initialize(TCHAR chVolume, LPSECURITY_ATTRIBUTES lpsa = NULL) throw()
	{
		TCHAR szVolumePath[_MAX_PATH];
		wsprintf(szVolumePath, TEXT("\\\\.\\%c:"), chVolume);

		return CDeviceControlBase<TAsync>::Initialize(szVolumePath,
			GENERIC_WRITE | GENERIC_READ, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, lpsa);
	}

	HRESULT Create(DWORDLONG dwlMaximumSize, DWORDLONG dwlAllocationDelta)
	{
		// This function creates a journal on the volume. If a journal already
		// exists this function will adjust the MaximumSize and AllocationDelta
		// parameters of the journal
		DWORD cb;
		CREATE_USN_JOURNAL_DATA cujd = { dwlMaximumSize, dwlAllocationDelta };

		BOOL bRetVal = DeviceIoControl(FSCTL_CREATE_USN_JOURNAL, 
			&cujd, sizeof(cujd), NULL, 0, &cb);

		return (!bRetVal) ? AtlHresultFromLastError() : S_OK;
	}

	HRESULT Delete(DWORD dwDeleteFlags)
	{
		// If DeleteFlags specifies USN_DELETE_FLAG_DELETE, this journal
		// will be deleted. If USN_DELETE_FLAG_NOTIFY is specified, the function
		// waits until the system has finished the delete process.
		// USN_DELETE_FLAG_NOTIFY can be specified alone to wait for the system
		// to finish if another application has started deleting a journal.

		if (m_ujd.UsnJournalID == 0)
		{
			HRESULT hr = Query(&m_ujd);
			if (FAILED(hr)) return hr;
		}

		DELETE_USN_JOURNAL_DATA dujd = { m_ujd.UsnJournalID, dwDeleteFlags };

		DWORD cb;
		return DeviceIoControl(FSCTL_DELETE_USN_JOURNAL, 
			&dujd, sizeof(dujd), NULL, 0, &cb);
	}

	HRESULT Query(PUSN_JOURNAL_DATA pUsnJournalData)
	{
		// Return statistics about the journal on the current volume
		DWORD cb;
		return DeviceIoControl(FSCTL_QUERY_USN_JOURNAL, NULL, 0,
			pUsnJournalData, sizeof(USN_JOURNAL_DATA), &cb, false);
	}

	HRESULT Seek(USN usn, DWORD dwReasonMask, DWORD dwlReturnOnlyOnClose)
	{
		// This function starts the process of reading data from the journal. The
		// parameters specify the initial location and filters to use when
		// reading the journal. The usn parameter may be zero to start reading
		// at the start of available data. The EnumNext function is called to
		// actualy get journal records

		if (m_ujd.UsnJournalID == 0)
		{
			HRESULT hr = Query(&m_ujd);
			if (FAILED(hr)) return hr;
		}

		// Store the parameters in m_rujd. This will determine
		// how we load buffers with the MoveNext function.
		m_rujd.UsnJournalID = m_ujd.UsnJournalID;
		m_rujd.StartUsn = (usn < m_ujd.LowestValidUsn) ?
			m_ujd.LowestValidUsn : usn;

		m_rujd.ReasonMask = dwReasonMask;
		m_rujd.ReturnOnlyOnClose = dwlReturnOnlyOnClose;

		m_rujd.Timeout = 0;
		m_rujd.BytesToWaitFor = 0;

		m_pUsnRecord = NULL;
		return MoveNext();
	}

	HRESULT MoveNext(BOOL bWait = FALSE)
	{
		PUSN_RECORD pNextUsnRecord = 
			m_pUsnRecord ? NextEntry(m_pUsnRecord) : NULL;

		if (pNextUsnRecord == 0 ||
			(PBYTE)pNextUsnRecord >= (m_pDataBuffer + m_nDataLength))
		{
			m_pUsnRecord = NULL;
			m_rujd.BytesToWaitFor = bWait ? sizeof(USN) : 0;

			HRESULT hr = DeviceIoControl(FSCTL_READ_USN_JOURNAL,
				&m_rujd, sizeof(READ_USN_JOURNAL_DATA), m_pDataBuffer,
				HeapSize(GetProcessHeap(), 0, m_pDataBuffer), &m_nDataLength);

			if (SUCCEEDED(hr))
			{
				// It is possible that DeviceIoControl succeeds, but has not
				// returned any records - this happens when we reach the end of
				// available journal records.

				// Set the last error to NO_ERROR so the caller can distinguish
				// between an error, and the case where no records were returned.
				SetLastError(NO_ERROR);

				// Store the 'next usn' into m_rujd.StartUsn for use the
				// next time we want to read from the journal
				m_ujd.NextUsn = m_rujd.StartUsn = *(USN*)m_pDataBuffer;

				// If we got more than sizeof(USN) bytes, we must have a record.
				// Point the current record to the first record in the buffer
				if (m_nDataLength > sizeof(USN))
					m_pUsnRecord = (PUSN_RECORD)&m_pDataBuffer[sizeof(USN)];

			}

			return hr;
		}

		m_pUsnRecord = pNextUsnRecord;
		return S_OK;
	}

	HRESULT EnumerateMft(USN usnLow, USN usnHigh)
	{
		// The next call uses the FRN in the first 8 bytes of the buffer
		DWORDLONG StartFRN = m_nDataLength >= sizeof(DWORDLONG) ?
			*((DWORDLONG*)m_pDataBuffer) : 0;

		// Enumerate MFT for files with 'Last USN' between usnLow and usnHigh
		MFT_ENUM_DATA med = { StartFRN, usnLow, usnHigh };

		HRESULT hr = DeviceIoControl(FSCTL_ENUM_USN_DATA, 
			&med, sizeof(MFT_ENUM_DATA), m_pDataBuffer,
			HeapSize(GetProcessHeap(), 0, m_pDataBuffer),
			&m_nDataLength, false);

		if (SUCCEEDED(hr))
		{
			m_ujd.NextUsn = m_rujd.StartUsn = *(USN*)m_pDataBuffer;
			m_pUsnRecord = (PUSN_RECORD)&m_pDataBuffer[sizeof(USN)];
		}
		return hr;
	}

	HRESULT MarkHandle(HANDLE hStream,
		DWORD dwUsnSourceInfo, DWORD dwHandleInfo)
	{
		if (hStream == NULL)
			return E_INVALIDARG;

		MARK_HANDLE_INFO mhi = { dwUsnSourceInfo, m_h, dwHandleInfo };
		// Marks a specified file or directory with information about changes
		DWORD cb;
		BOOL bRetVal = ::DeviceIoControl(hStream, FSCTL_MARK_HANDLE,
			&mhi, sizeof(MARK_HANDLE_INFO), NULL, 0, &cb, NULL);

		return (!bRetVal) ? AtlHresultFromLastError() : S_OK;
	}

	static HRESULT ReadFileUsn(HANDLE hFile, PUSN_RECORD& pUsnRecord)
	{
		if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
			return E_INVALIDARG;

		static BYTE buffer[4096]; // buffer to hold single record
		HRESULT hr = NOERROR;

		// Get journal related information regarding the open
		// file or directory specified by 'hFile'
		DWORD cb;
		if (!::DeviceIoControl(hFile, FSCTL_READ_FILE_USN_DATA,
			NULL, 0, buffer, sizeof(buffer), &cb, NULL))
			hr = AtlHresultFromLastError();

		pUsnRecord = (PUSN_RECORD)buffer;
		return hr;
	}

	static HRESULT WriteCloseRecord(HANDLE hFile, USN& usn)
	{
		if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
			return E_INVALIDARG;

		// Force a close record for the open file specified by 'hFile'
		DWORD cb;
		if (!::DeviceIoControl(hFile, FSCTL_WRITE_USN_CLOSE_RECORD,
			NULL, 0, &usn, sizeof(usn), &cb, NULL))
			return AtlHresultFromLastError();

		return S_OK;
	}

	static HRESULT GetFileReferenceNumber(HANDLE hFile, DWORDLONG& frn)
	{
		if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
			return E_INVALIDARG;

		BY_HANDLE_FILE_INFORMATION fi;
		if (!GetFileInformationByHandle(hFile, &fi))
			return AtlHresultFromLastError();

		frn = (((DWORDLONG)fi.nFileIndexHigh) > 32) | fi.nFileIndexLow; 
		return S_OK;
	}

	DWORDLONG GetCurrentUsnJournalID(DWORD dwMilliseconds = 10000)
	{
		if (m_ujd.UsnJournalID == 0 && Query(&m_ujd) == E_IO_PENDING)
			::WaitForSingleObject(GetEventHandle(), dwMilliseconds);

		return m_ujd.UsnJournalID;
	}

	USN GetStartUsn() { return m_rujd.StartUsn; }
	PUSN_RECORD GetCurrentRecord() { return m_pUsnRecord; }

public:  // Public static helper functions
	static BOOL GetDateTimeFormatFromFileTime(__int64 filetime,
		LPTSTR pszDateTime, int cchDateTime);
	static BOOL GetReasonString(DWORD dwReason, LPTSTR pszReason,
		int cchReason);

private:
	PUSN_RECORD NextEntry(const PUSN_RECORD pUsnRecord)
	{
		DWORD dwNextUsnRecord = (DWORD)pUsnRecord;
		dwNextUsnRecord += pUsnRecord->RecordLength;

		// Round up the record size to match the 64-bit alinment, if
		// the size is not already a multiple of 8...

		if (dwNextUsnRecord & 8 - 1)
		{
			dwNextUsnRecord &= -8; // round down to nearest multiple of 8
			dwNextUsnRecord += 8;
		}
		return (PUSN_RECORD)dwNextUsnRecord;
	}
/*
	DWORD GetMaxRecordSize()
	{
//		GetVolumeInformation()
		return (255 * sizeof(WCHAR) + sizeof(USN_RECORD));
	}
*/
private:
	// Members used to enumerate journal records
	READ_USN_JOURNAL_DATA m_rujd; // parameters for reading records
	USN_JOURNAL_DATA m_ujd;
	PBYTE m_pDataBuffer; // buffer of records
	DWORD m_nDataLength; // valid bytes in buffer
	PUSN_RECORD m_pUsnRecord; // pointer to current record
};

HRESULT ReadFileUsn(LPCTSTR szFilePath, PUSN_RECORD& pUsnRecord)
{
	HANDLE hFile = ::CreateFile(szFilePath, GENERIC_READ,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return AtlHresultFromLastError();

	HRESULT hr = CChangeJornal<>::ReadFileUsn(hFile, pUsnRecord);
	CloseHandle(hFile);
	return hr;
}

HRESULT WriteCloseRecord(LPCTSTR szFilePath, USN& usn)
{
	HANDLE hFile = ::CreateFile(szFilePath, GENERIC_READ,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return AtlHresultFromLastError();

	HRESULT hr = CChangeJornal<>::WriteCloseRecord(hFile, usn);
	CloseHandle(hFile);
	return hr;
}

}; //namespace WETLA

#endif // __WETLA_FILESTG_H__
