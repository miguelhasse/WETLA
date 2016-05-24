#ifndef __WETLA_DDX_H__
#define __WETLA_DDX_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_frame.h requires atlapp.h to be included first
#endif

#ifndef __ATLDDX_H__
	#error wetla_frame.h requires atlddx.h to be included first
#endif

namespace WETLA
{

// Constants
#define DDX_LOAD	FALSE
#define DDX_SAVE	TRUE

// DDX map macros

#define DDX_TEXT(nID, var) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Text(nID, var, sizeof(var), bSaveAndValidate);

#define DDX_TEXT_LEN(nID, var, len) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Text(nID, var, sizeof(var), bSaveAndValidate, TRUE, len);

#define DDX_INT(nID, var) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Int(nID, var, TRUE, bSaveAndValidate);

#define DDX_INT_RANGE(nID, var, min, max) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Int(nID, var, TRUE, bSaveAndValidate, TRUE, min, max);

#define DDX_UINT(nID, var) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Int(nID, var, FALSE, bSaveAndValidate);

#define DDX_UINT_RANGE(nID, var, min, max) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Int(nID, var, FALSE, bSaveAndValidate, TRUE, min, max);

#ifdef _ATL_USE_DDX_FLOAT
#define DDX_FLOAT(nID, var) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Float(nID, var, bSaveAndValidate));

#define DDX_FLOAT_RANGE(nID, var, min, max) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Float(nID, var, bSaveAndValidate, TRUE, min, max);

#define DDX_FLOAT_P(nID, var, precision) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Float(nID, var, bSaveAndValidate, FALSE, 0, 0, precision);

#define DDX_FLOAT_P_RANGE(nID, var, min, max, precision) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			WTL::DDX_Float(nID, var, bSaveAndValidate, TRUE, min, max, precision);
#endif //_ATL_USE_DDX_FLOAT

#define DDX_LB_STRING(nID, var) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			DDX_LBString(nID, var, sizeof(var), bSaveAndValidate);

#define DDX_LB_STRING_LEN(nID, var, len) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			DDX_LBString(nID, var, sizeof(var), bSaveAndValidate, TRUE, len);

#define DDX_LB_EXACT_STRING(nID, var) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			DDX_LBString(nID, var, sizeof(var), bSaveAndValidate);

#define DDX_LB_EXACT_STRING_LEN(nID, var, len) \
		if (nCtlID == (UINT)-1 || nCtlID == nID) \
			DDX_LBString(nID, var, sizeof(var), bSaveAndValidate, TRUE, len);

///////////////////////////////////////////////////////////////////////////////
// CWinDataExchange2 - provides extended support for DDX

template <class T>
class CWinDataExchangeEx : WTL::CWinDataExchange<T>
{
public:
// Listbox string exchange
	BOOL DDX_LBString(UINT nID, LPTSTR lpstrText, int cbSize, BOOL bSave, BOOL bValidate = FALSE, int nLength = 0)
	{
		T* pT = static_cast<T*>(this);
		HWND hWndCtrl = pT->GetDlgItem(nID);
		BOOL bSuccess = TRUE;

		if (bSave)
		{
			int nIndex = (int)::SendMessage(hWndCtrl, LB_GETCURSEL, 0, 0L);
			if (nIndex != -1)
			{
				int nRetLen = (int)::SendMessage(hWndCtrl, LB_GETTEXTLEN, nIndex, 0L);

				LPTSTR lpstrBuffer = (nRetLen < cbSize / sizeof(TCHAR)) ?
					lpstrText : new TCHAR[nRetLen + 1];

				if (::SendMessage(hWndCtrl, LB_GETTEXT, nIndex, 
					(LPARAM)(LPVOID)lpstrBuffer) < cbSize / sizeof(TCHAR))
					bSuccess = FALSE;

				if (lpstrBuffer != lpstrText)
				{
					lstrcpyn(lpstrText, lpstrBuffer, cbSize / sizeof(TCHAR));
					delete[] lpstrBuffer;
				}
			}
		}
		else
		{
			ATLASSERT(!bValidate || lstrlen(lpstrText) <= nLength);
			bSuccess = (::SendMessage(hWndCtrl, LB_SELECTSTRING,
				(WPARAM)-1, (LPARAM)(LPCTSTR)lpstrText) != LB_ERR);
		}

		if (!bSuccess)
		{
			pT->OnDataExchangeError(nID, bSave);
		}
		else if (bSave && bValidate)   // validation
		{
			ATLASSERT(nLength > 0);
			if (lstrlen(lpstrText) > nLength)
			{
				_XData data = { ddxDataText };
				data.textData.nLength = lstrlen(lpstrText);
				data.textData.nMaxLength = nLength;
				pT->OnDataValidateError(nID, bSave, data);
				bSuccess = FALSE;
			}
		}
		return bSuccess;
	}

	BOOL DDX_LBStringExact(UINT nID, LPTSTR lpstrText, int cbSize, BOOL bSave, BOOL bValidate = FALSE, int nLength = 0)
	{
		T* pT = static_cast<T*>(this);
		BOOL bSuccess = TRUE;

		if (bSave)
		{
			bSuccess = DDX_LBString(nID, lpstrText, cbSize, bSave, bValidate, nLength);
		}
		else
		{
			ATLASSERT(!bValidate || lstrlen(lpstrText) <= nLength);
			HWND hWndCtrl = pT->GetDlgItem(nID);
			int nIndex = (int)::SendMessage(hWndCtrl, LB_FINDSTRINGEXACT,
				(WPARAM)-1, (LPARAM)(LPCTSTR)lpstrText)

			bSuccess = (nIndex != LB_ERR && ::SendMessage(hWndCtrl, LB_SETCURSEL,
				(WPARAM)nIndex, (LPARAM)NULL) != LB_ERR);

			if (!bSuccess)
			{
				pT->OnDataExchangeError(nID, bSave);
			}
			else if (bSave && bValidate)   // validation
			{
				ATLASSERT(nLength > 0);
				if (lstrlen(lpstrText) > nLength)
				{
					_XData data = { ddxDataText };
					data.textData.nLength = lstrlen(lpstrText);
					data.textData.nMaxLength = nLength;
					pT->OnDataValidateError(nID, bSave, data);
					bSuccess = FALSE;
				}
			}
		}
		return bSuccess;
	}

#ifdef __ATLCOMTIME_H__
	BOOL DDX_MCDateTime(UINT nID, COleDateTime& dtDateTime, BOOL bSave)
	{
		T* pT = static_cast<T*>(this);
		BOOL bSuccess = TRUE;

		if (bSave)
		{
			SYSTEMTIME sysTime;
			if (bSuccess = (::SendMessage(hWndCtrl, DTM_GETSYSTEMTIME, 0,
				(LPARAM)(LPSYSTEMTIME)&sysTime) != 0))
				dtDateTime = sysTime;
			else dtDateTime.SetStatus(COleDateTime::null);
		}
		else
		{
			SYSTEMTIME sysTime;
			if (bSuccess = dtDateTime.GetAsSystemTime(sysTime))
			{
				bSuccess = (::SendMessage(hWndCtrl, DTM_SETSYSTEMTIME, 
					(sysTime.wMonth > 0 && sysTime.wMonth < 13) ? GDT_VALID : GDT_NONE,
					(LPARAM)(LPSYSTEMTIME)&sysTime) != 0);
			}
		}
		return bSuccess;
	}
#endif //__ATLCOMTIME_H__

	BOOL DDX_MCDateTime(UINT nID, SYSTEMTIME& sysTime, BOOL bSave)
	{
		T* pT = static_cast<T*>(this);
		BOOL bSuccess = TRUE;

		if (bSave)
		{
			bSuccess = (::SendMessage(hWndCtrl, DTM_GETSYSTEMTIME, 0,
				(LPARAM)(LPSYSTEMTIME)&sysTime) != 0);
		}
		else
		{
			bSuccess = (::SendMessage(hWndCtrl, DTM_SETSYSTEMTIME, 
				(sysTime.wMonth > 0 && sysTime.wMonth < 13) ? GDT_VALID : GDT_NONE,
				(LPARAM)(LPSYSTEMTIME)&sysTime) != 0);
		}
		return bSuccess;
	}
};

}; //namespace WETLA

#endif // __WETLA_DDX_H__
