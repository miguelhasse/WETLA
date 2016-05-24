#ifndef __WETLA_SCILEX_H__
#define __WETLA_SCILEX_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCTRLS_H__
   #error wetla_scilex.h requires atlctrls.h to be included first
#endif

#if (_WIN32_IE < 0x0300)
   #error wetla_scilex.h requires IE Version 3.0 or higher
#endif

#ifndef STATIC_BUILD
#include <scintilla.h>
#pragma comment(lib, "scilexer.lib")
extern "C" __declspec(dllimport) LRESULT __stdcall Scintilla_DirectFunction(
    LPVOID ptr, UINT Msg, WPARAM wParam, LPARAM lParam);
#else
#define PLAT_WIN 1
#include <scintilla.h>
//LRESULT Scintilla_DirectFunction(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
extern "C" LRESULT __stdcall Scintilla_DirectFunction(LPVOID ptr, UINT Msg, WPARAM wParam, LPARAM lParam);
#undef PLAT_WIN
#endif

namespace WETLA
{

///////////////////////////////////////////////////////////////////////////////
// CScintillaCtrlT

template <class TBase>
class CScintillaCtrlT : public TBase
{
// Constructors
public:
	CScintillaCtrlT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CScintillaCtrlT< TBase >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		return TBase::Create(GetWndClassName(), hWndParent, rect.m_lpRect,
			szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
	}

	LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const throw()
	{
	#ifndef STATIC_BUILD
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, message, wParam, lParam);
	#else
		ATLASSERT(::IsWindow(m_hWnd));
		return Scintilla_DirectFunction(reinterpret_cast<LPVOID>(
			::GetWindowLongPtr(m_hWnd, 0)), message, wParam, lParam);
	#endif
	}

// Attributes
public:
	static LPCTSTR GetWndClassName()
	{
		return TEXT("Scintilla");
	}

// Text retrieval and modification
public:
	int GetText(LPSTR pszText, int nLength)
	{
		return (int)SendMessage(SCI_GETTEXT, (WPARAM)nLength, (LPARAM)pszText);
	}
	BOOL SetText(LPCSTR pszText)
	{
		return (BOOL)SendMessage(SCI_SETTEXT, 0, (LPARAM)pszText);
	}
	void SetSavePoint()
	{
		SendMessage(SCI_SETSAVEPOINT, 0, 0L);
	}
	int GetLine(int nLine, LPSTR pszText)
	{
		return (int)SendMessage(SCI_GETLINE, (WPARAM)nLine, (LPARAM)pszText);
	}
	void ReplaceSelection(LPCSTR pszText)
	{
		SendMessage(SCI_REPLACESEL, 0, (LPARAM)pszText);
	}
	BOOL SetReadOnly(bool bReadOnly)
	{
		return (BOOL)SendMessage(SCI_SETREADONLY, (WPARAM)readOnly, 0L);
	}
	BOOL GetReadOnly() const
	{
		return (BOOL)SendMessage(SCI_GETREADONLY, 0, 0L);
	}
	int GetTextRange(TEXTRANGE* lpTextRange)
	{
		return (int)SendMessage(SCI_GETTEXTRANGE, 0, (LPARAM)lpTextRange);
	}
	int GetTextRange(LPSTR pszText, int cpMin, int cpMax = -1)
	{
		TEXTRANGE tr = { {(LONG)cpMin, (LONG)cpMax}, pszText };
		return (int)SendMessage(SCI_GETTEXTRANGE, 0, (LPARAM)&tr);
	}
	void Allocate(UINT nLength)
	{
		SendMessage(SCI_ALLOCATE, (WPARAM)nLength, 0L);
	}
	void AddText(LPCSTR pszText, int nLength)
	{
		SendMessage(SCI_ADDTEXT, (WPARAM)nLength, (LPARAM)pszText);
	}
	void AddStyledText(LPCSTR pszText, int nLength)
	{
		SendMessage(SCI_ADDSTYLEDTEXT, (WPARAM)nLength, (LPARAM)pszText);
	}
	void AppendText(LPCSTR pszText, int nLength)
	{
		SendMessage(SCI_APPENDTEXT, (WPARAM)nLength, (LPARAM)pszText);
	}
	void InsertText(LPCSTR pszText, int nPosition = 0)
	{
		SendMessage(SCI_INSERTTEXT, (WPARAM)nPosition, (LPARAM)pszText);
	}
	void ClearAll()
	{
		SendMessage(SCI_CLEARALL, 0, 0L);
	}
	void ClearDocumentStyle()
	{
		SendMessage(SCI_CLEARDOCUMENTSTYLE, 0, 0L);
	}
	CHAR GetCharAt(int nPosition) const
	{
		return (CHAR)SendMessage(SCI_GETCHARAT, (WPARAM)nPosition, 0L);
	}
	BYTE GetStyleAt(int nPosition) const
	{
		return (BYTE)SendMessage(SCI_GETSTYLEAT, (WPARAM)nPosition, 0L);
	}
	int GetStyledText(TextRange* lpTextRange)
	{
		return (int)SendMessage(SCI_GETSTYLEDTEXT, 0, (LPARAM)lpTextRange);
	}
	int GetStyledText(LPSTR pszText, int cpMin, int cpMax = -1)
	{
		TEXTRANGE tr = { {(LONG)cpMin, (LONG)cpMax}, pszText };
		return (int)SendMessage(SCI_GETSTYLEDTEXT, 0, (LPARAM)&tr);
	}
	void SetStyleBits(DWORD dwStyleBits)
	{
		SendMessage(SCI_SETSTYLEBITS, (WPARAM)dwStyleBits, 0L);
	}
	DWORD GetStyleBits() const
	{
		return (DWORD)SendMessage(SCI_GETSTYLEBITS, 0, 0L);
	}

// Searching and replacing
public:
	int FindText(TextToFind* lpFindRange, DWORD dwFlags)
	{
		return (int)SendMessage(SCI_FINDTEXT, (WPARAM)dwFlags, (LPARAM)lpFindRange);
	}
	int FindText(LPSTR pszPattern, int* lpcpMin, int* lpcpMax, DWORD dwFlags)
	{
		TextToFind ttf = {{(LONG)*lpcpMin, (LONG)*lpcpMax}, pszPattern, {0L, 0L}};
		LRESULT lRes = SendMessage(SCI_FINDTEXT, (WPARAM)dwFlags, (LPARAM)&ttf);
		*lpcpMin = (int)ttf.chrgText.cpMin; *lpcpMax = (int)ttf.chrgText.cpMax;
		return (int)lRes;
	}
	void SearchAnchor()
	{
		SendMessage(SCI_SEARCHANCHOR, 0, 0L);
	}
	int SearchText(LPCSTR pszText, DWORD dwSearchFlags)
	{
		return (int)SendMessage(SCI_SEARCHNEXT, (WPARAM)dwSearchFlags, (LPARAM)pszText);
	}
	void SetTargetStart(int nPosition)
	{
		SendMessage(SCI_SETTARGETSTART, (WPARAM)nPosition, 0L);
	}
	int GetTargetStart() const
	{
		return (int)SendMessage(SCI_GETTARGETSTART, 0, 0L);
	}
	void SetTargetEnd(int nPosition)
	{
		SendMessage(SCI_SETTARGETEND, (WPARAM)nPosition, 0L);
	}
	int GetTargetEnd() const
	{
		return (int)SendMessage(SCI_GETTARGETEND, 0, 0L);
	}
	void TargetFormSelection()
	{
		SendMessage(SCI_TARGETFROMSELECTION, 0, 0L);
	}
	void SetSearchFlags(DWORD dwFlags)
	{
		SendMessage(SCI_SETSEARCHFLAGS, (WPARAM)dwFlags, 0L);
	}
	DWORD GetSearchFlags() const
	{
		return (DWORD)SendMessage(SCI_GETSEARCHFLAGS, 0, 0L);
	}
	int SearchInTarget(LPCSTR pszText, int nLength)
	{
		return (int)SendMessage(SCI_SEARCHINTARGET, (WPARAM)nLength, (LPARAM)pszText);
	}
	int ReplaceTarget(LPCSTR pszText, int nLength)
	{
		return (int)SendMessage(SCI_REPLACETARGET, (WPARAM)nLength, (LPARAM)pszText);
	}
	int ReplaceTargetPattern(LPCSTR pszText, int nLength)
	{
		return (int)SendMessage(SCI_REPLACETARGETRE, (WPARAM)nLength, (LPARAM)pszText);
	}

// Overtype
public:
	void SetOverType(BOOL bOverType)
	{
		SendMessage(SCI_SETOVERTYPE, (WPARAM)bOverType, 0L);
	}
	BOOL GetOverType() const
	{
		return (BOOL)SendMessage(SCI_GETOVERTYPE, 0, 0L);
	}

// Cut, copy and paste
public:
	void Cut()
	{
		SendMessage(SCI_CUT, 0, 0L);
	}
	void Copy()
	{
		SendMessage(SCI_COPY, 0, 0L);
	}
	void Paste()
	{
		SendMessage(SCI_PASTE, 0, 0L);
	}
	void Clear()
	{
		SendMessage(SCI_CLEAR, 0, 0L);
	}
	BOOL CanPaste() const
	{
		return (BOOL)SendMessage(SCI_CANPASTE, 0, 0L);
	}
	void CopyRange(int nStartChar, int nEndChar)
	{
		SendMessage(SCI_COPYRANGE, (WPARAM)nStartChar, (LPARAM)nEndChar);
	}
	void CopyText(LPCSTR pszText, int nLength)
	{
		SendMessage(SCI_COPYTEXT, (WPARAM)nLength, (LPARAM)pszText);
	}

// Error handling
public:
	void Status(DWORD dwStatus)
	{
		SendMessage(SCI_SETSTATUS, (WPARAM)dwStatus, 0L);
	}
	DWORD Status() const
	{
		return (DWORD)SendMessage(SCI_GETSTATUS, 0, 0L);
	}

// Undo and Redo
public:
	void Undo()
	{
		SendMessage(SCI_UNDO, 0, 0L);
	}
	BOOL CanUndo() const
	{
		return (BOOL)SendMessage(SCI_CANUNDO, 0, 0L);
	}
	void EmptyUndoBuffer()
	{
		SendMessage(SCI_EMPTYUNDOBUFFER, 0, 0L);
	}
	void Redo()
	{
		SendMessage(SCI_REDO, 0, 0L);
	}
	BOOL CanRedo() const
	{
		return (BOOL)SendMessage(SCI_CANREDO, 0, 0L);
	}
	void SetUndoCollection(BOOL bCollectUndo)
	{
		SendMessage(SCI_SETUNDOCOLLECTION, (WPARAM)bCollectUndo, 0L);
	}
	BOOL GetUndoCollection() const
	{
		return (BOOL)SendMessage(SCI_GETUNDOCOLLECTION, 0, 0L);
	}
	void BeginUndoAction()
	{
		SendMessage(SCI_BEGINUNDOACTION, 0, 0L);
	}
	void EndUndoAction()
	{
		SendMessage(SCI_ENDUNDOACTION, 0, 0L);
	}

// Selection and information
public:
	int GetTextLength() const
	{
		return (int)SendMessage(SCI_GETTEXTLENGTH, 0, 0L);
	}
	int GetLength() const
	{
		return (int)SendMessage(SCI_GETLENGTH, 0, 0L);
	}
	int GetLineCount() const
	{
		return (int)SendMessage(SCI_GETLINECOUNT, 0, 0L);
	}
	int GetFirstVisibleLine() const
	{
		return (int)SendMessage(SCI_GETFIRSTVISIBLELINE, 0, 0L);
	}
	int GetLinesOnScreen() const
	{
		return (int)SendMessage(SCI_LINESONSCREEN, 0, 0L);
	}
	BOOL IsModified() const
	{
		return (BOOL)SendMessage(SCI_GETMODIFY, 0, 0L);
	}
	void SetSelection(int nStartChar = -1, int nEndChar = -1)
	{
		SendMessage(SCI_SETSEL, (WPARAM)nStartChar, (LPARAM)nEndChar);
	}
	void SetSelection(DWORD dwSelection)
	{
		SendMessage(SCI_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection));
	}
	int SetSelection(LONG nStartChar, LONG nEndChar)
	{
		CHARRANGE cr = { nStartChar, nEndChar };
		return (int)SendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
	}
	void GotoPosition(int nPosition = -1)
	{
		SendMessage(SCI_GOTOPOS, (WPARAM)nPosition, 0L);
	}
	void GotoLine(int nLine = -1)
	{
		SendMessage(SCI_GOTOLINE, (WPARAM)nLine, 0L);
	}
	void SetCurrentPosition(int nPosition = -1)
	{
		SendMessage(SCI_SETCURRENTPOS, (WPARAM)nPosition, 0L);
	}
	int GetCurrentPosition() const
	{
		return (int)SendMessage(SCI_GETCURRENTPOS, 0, 0L);
	}
	void SetAnchor(int nPosition = -1)
	{
		SendMessage(SCI_SETANCHOR, (WPARAM)nPosition, 0L);
	}
	int GetAnchor() const
	{
		return (int)SendMessage(SCI_GETANCHOR, 0, 0L);
	}
	void SetSelectionStart(int nPosition = -1)
	{
		SendMessage(SCI_SETSELECTIONSTART, (WPARAM)nPosition, 0L);
	}
	int GetSelectionStart() const
	{
		return (int)SendMessage(SCI_GETSELECTIONSTART, 0, 0L);
	}
	void SetSelectionEnd(int nPosition = -1)
	{
		SendMessage(SCI_SETSELECTIONEND, (WPARAM)nPosition, 0L);
	}
	int GetSelectionEnd() const
	{
		return (int)SendMessage(SCI_GETSELECTIONEND, 0, 0L);
	}
	void SelectAll()
	{
		SendMessage(SCI_SELECTALL, 0, 0L);
	}
	int LineFromPosition(int nPosition) const
	{
		return (int)SendMessage(SCI_LINEFROMPOSITION, (WPARAM)nPosition, 0L);
	}
	int PositionFromLine(int nLine) const
	{
		return (int)SendMessage(SCI_POSITIONFROMLINE, (WPARAM)nLine, 0L);
	}
	int GetLineEndPosition(int nLine) const
	{
		return (int)SendMessage(SCI_GETLINEENDPOSITION, (WPARAM)nLine, 0L);
	}
	int GetLineLength(int nLine) const
	{
		return (int)SendMessage(SCI_LINELENGTH, (WPARAM)nLine, 0L);
	}
	int GetColumn(int nPosition) const
	{
		return (int)SendMessage(SCI_GETCOLUMN, (WPARAM)nPosition, 0L);
	}
	int FindColumn(int nLine, int nColumn) const
	{
		return (int)SendMessage(SCI_FINDCOLUMN, (WPARAM)nLine, (LPARAM)nColumn);
	}
	int PositionFromPoint(int x, int y) const
	{
		return (int)SendMessage(SCI_POSITIONFROMPOINT, (WPARAM)x, (LPARAM)y);
	}
	int PositionClosestFromPoint(int x, int y) const
	{
		return (int)SendMessage(SCI_POSITIONFROMPOINTCLOSE, (WPARAM)x, (LPARAM)y);
	}
	int PointXFromPosition(int nPosition) const
	{
		return (int)SendMessage(SCI_POINTXFROMPOSITION, (WPARAM)nPosition, 0L);
	}
	int PointYFromPosition(int nPosition) const
	{
		return (int)SendMessage(SCI_POINTYFROMPOSITION, (WPARAM)nPosition, 0L);
	}
	//POINT PointFromPosition(int nPosition) const
	//{
	//	HRESULT hRes = SendMessage(SCI_POINTFROMPOSITION, (WPARAM)nPosition, 0L);
	//	return { GET_X_LPARAM(hRes),GET_Y_LPARAM(hRes) };
	//}
	void HideSelection(BOOL bHide)
	{
		SendMessage(SCI_HIDESELECTION, (WPARAM)bHide, 0L);
	}
	int GetSelectedText(LPSTR pszText)
	{
		return (int)SendMessage(SCI_GETSELTEXT, 0, (LPARAM)pszText);
	}
	int GetCurrentLine(LPSTR pszText, int nLength)
	{
		return (int)SendMessage(SCI_GETCURLINE, (WPARAM)nLength, (LPARAM)pszText);
	}
	BOOL IsSelectionRectangle() const
	{
		return (BOOL)SendMessage(SCI_SELECTIONISRECTANGLE, (WPARAM)nPosition, 0L);
	}
	SHORT SetSelectionMode(SHORT nMode)
	{
		return (SHORT)SendMessage(SCI_SETSELECTIONMODE, (WPARAM)nMode, 0L);
	}
	SHORT GetSelectionMode() const
	{
		return (SHORT)SendMessage(SCI_GETSELECTIONMODE, (WPARAM)nMode, 0L);
	}
	int GetSelectionLineStartPosition(int nLine) const
	{
		return (int)SendMessage(SCI_GETLINESELSTARTPOSITION, (WPARAM)nLine, 0L);
	}
	int GetSelectionLineEndPosition(int nLine) const
	{
		return (int)SendMessage(SCI_GETLINESELENDPOSITION, (WPARAM)nLine, 0L);
	}
	void MoveCaretInsideView()
	{
		SendMessage(SCI_MOVECARETINSIDEVIEW, 0, 0L);
	}
	int GetWordStartPosition(int nPosition, BOOL bOnlyWordCharacters) const
	{
		return (int)SendMessage(SCI_WORDSTARTPOSITION, (WPARAM)nPosition, (LPARAM)bOnlyWordCharacters);
	}
	int GetWordeEndPosition(int nPosition, BOOL bOnlyWordCharacters) const
	{
		return (int)SendMessage(SCI_WORDENDPOSITION, (WPARAM)nPosition, (LPARAM)bOnlyWordCharacters);
	}
	int PositionBefore(int nPosition) const
	{
		return (int)SendMessage(SCI_POSITIONBEFORE, (WPARAM)nPosition, 0L);
	}
	int PositionAfter(int nPosition) const
	{
		return (int)SendMessage(SCI_POSITIONAFTER, (WPARAM)nPosition, 0L);
	}
	int GetTextWidth(LPSTR pszText, int nStyleNumber)
	{
		return (int)SendMessage(SCI_TEXTWIDTH, (WPARAM)nStyleNumber, (LPARAM)pszText);
	}
	int GetTextHeight(int nLine)
	{
		return (int)SendMessage(SCI_TEXTHEIGHT, (WPARAM)nLine, 0L);
	}
	void ChooseCaretX()
	{
		SendMessage(SCI_CHOOSECARETX, 0, 0L);
	}
	int LineFromChar(int nIndex = -1) const
	{
		return (int)SendMessage(EM_LINEFROMCHAR, nIndex, 0L);
	}
	int LineFromCharEx(LONG nIndex) const
	{
		return (int)SendMessage(EM_EXLINEFROMCHAR, 0, (LPARAM)nIndex);
	}
	DWORD GetSelection() const
	{
		return (DWORD)SendMessage(EM_GETSEL, 0, 0L);
	}
	void GetSelection(int& nStartChar, int& nEndChar) const
	{
		SendMessage(EM_GETSEL, (WPARAM)&nStartChar, (LPARAM)&nEndChar);
	}
	void GetSelection(CHARRANGE& cr) const
	{
		SendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
	}

// Scrolling and automatic scrolling
public:
	void LineScroll(int nLine, int nColumn)
	{
		SendMessage(SCI_LINESCROLL, (WPARAM)nColumn, (LPARAM)nLine);
	}
	void EnsureCaretVisible()
	{
		SendMessage(SCI_SCROLLCARET, 0, 0L);
	}
	void SetCaretXPolicy(int nCaretPolicy, int nCaretSlop)
	{
		SendMessage(SCI_SETXCARETPOLICY, (WPARAM)nCaretPolicy, (LPARAM)nCaretSlop);
	}
	void SetCaretYPolicy(int nCaretPolicy, int nCaretSlop)
	{
		SendMessage(SCI_SETYCARETPOLICY, (WPARAM)nCaretPolicy, (LPARAM)nCaretSlop);
	}
	void SetVisiblePolicy(int nCaretPolicy, int nCaretSlop)
	{
		SendMessage(SCI_SETVISIBLEPOLICY, (WPARAM)nCaretPolicy, (LPARAM)nCaretSlop);
	}
	void SetHScrollBar(BOOL bShow)
	{
		SendMessage(SCI_SETHSCROLLBAR, (WPARAM)bShow, 0L);
	}
	BOOL GetHScrollBar() const
	{
		return (BOOL)SendMessage(SCI_GETHSCROLLBAR, 0, 0L);
	}
	void SetVScrollBar(BOOL bShow)
	{
		SendMessage(SCI_SETVSCROLLBAR, (WPARAM)bShow, 0L);
	}
	BOOL GetVScrollBar() const
	{
		return (BOOL)SendMessage(SCI_GETVSCROLLBAR, 0, 0L);
	}
	void SetXOffset(int nOffset)
	{
		SendMessage(SCI_SETXOFFSET, (WPARAM)nOffset, 0L);
	}
	int GetXOffset() const
	{
		return (int)SendMessage(SCI_GETXOFFSET, 0, 0L);
	}
	void SetScrollWidth(int nOffset)
	{
		SendMessage(SCI_SETSCROLLWIDTH, (WPARAM)nOffset, 0L);
	}
	int GetScrollWidth() const
	{
		return (int)SendMessage(SCI_GETSCROLLWIDTH, 0, 0L);
	}
	void SetEndAtLastLine(BOOL bEndAtLastLine)
	{
		SendMessage(SCI_SETENDATLASTLINE, (WPARAM)bEndAtLastLine, 0L);
	}
	BOOL GetEndAtLastLine() const
	{
		return (BOOL)SendMessage(SCI_GETENDATLASTLINE, 0, 0L);
	}

// White space
public:
	void SetWhitespaceMode(SHORT nMode)
	{
		SendMessage(SCI_SETVIEWWS, (WPARAM)bEndAtLastLine, 0L);
	}
	SHORT GetWhitespaceMode() const
	{
		return (SHORT)SendMessage(SCI_GETVIEWWS, 0, 0L);
	}
	void SetWhitespaceForeground(COLORREF color, BOOL bUseSetting)
	{
		SendMessage(SCI_STYLESETFORE, (WPARAM)bUseSetting, (LPARAM)color);
	}
	void SetWhitespaceBackground(COLORREF color, BOOL bUseSetting)
	{
		SendMessage(SCI_STYLESETFORE, (WPARAM)bUseSetting, (LPARAM)color);
	}

// Cursor
public:
	void SetCursor(SHORT nType)
	{
		SendMessage(SCI_SETCURSOR, (WPARAM)nType, 0L);
	}
	SHORT GetCursor() const
	{
		return (SHORT)SendMessage(SCI_GETCURSOR, 0, 0L);
	}

// Mouse capture
public:
	void SetMouseDownCapture(BOOL bCapture)
	{
		SendMessage(SCI_SETMOUSEDOWNCAPTURES, (WPARAM)bCapture, 0L);
	}
	BOOL GetMouseDownCapture() const
	{
		return (BOOL)SendMessage(SCI_GETMOUSEDOWNCAPTURES, 0, 0L);
	}

// Line endings
public:
	void SetEOLMode(int nMode)
	{
		SendMessage(SCI_SETEOLMODE, (WPARAM)nMode, 0L);
	}
	int GetEOLMode() const
	{
		return (int)SendMessage(SCI_GETEOLMODE, 0, 0L);
	}
	void ConvertEOLs(int nMode)
	{
		SendMessage(SCI_CONVERTEOLS, (WPARAM)nMode, 0L);
	}
	void SetViewEOL(BOOL bShow)
	{
		SendMessage(SCI_SETVIEWEOL, (WPARAM)bShow, 0L);
	}
	BOOL GetViewEOL() const
	{
		return (BOOL)SendMessage(SCI_GETVIEWEOL, 0, 0L);
	}

// Styling
public:
	int GetEndStyled() const
	{
		return (int)SendMessage(SCI_GETENDSTYLED, 0, 0L);
	}
	void StartStyling(int nPosition, DWORD dwMask)
	{
		SendMessage(SCI_STARTSTYLING, (WPARAM)nPosition, (LPARAM)dwMask);
	}
	void SetStyling(int nLength, SHORT nStyleNumber)
	{
		SendMessage(SCI_SETSTYLING, (WPARAM)nLength, (LPARAM)nStyleNumber);
	}
	void SetStyling(int nLength, LPCSTR pszStyles)
	{
		SendMessage(SCI_SETSTYLINGEX, (WPARAM)nLength, (LPARAM)pszStyles);
	}
	void SetLineState(int nLine, DWORD dwValue)
	{
		SendMessage(SCI_SETLINESTATE, (WPARAM)nLine, (LPARAM)dwValue);
	}
	int GetLineState(int nLine) const
	{
		return (int)SendMessage(SCI_GETLINESTATE, (WPARAM)nLine, 0L);
	}
	int GetMaxLineState() const
	{
		return (int)SendMessage(SCI_GETMAXLINESTATE, 0, 0L);
	}

// Style definition
public:
	void SetStyleResetDefault()
	{
		SendMessage(SCI_STYLERESETDEFAULT, 0, 0L);
	}
	void SetStyleClearAll()
	{
		SendMessage(SCI_STYLECLEARALL, 0, 0L);
	}
	void SetStyleFont(SHORT nStyleNumber, LPCSTR pszFontName)
	{
		SendMessage(SCI_STYLESETFONT, (WPARAM)nStyleNumber, (LPARAM)pszFontName);
	}
	void SetStyleSize(SHORT nStyleNumber, LONG fPointSize)
	{
		SendMessage(SCI_STYLESETSIZE, (WPARAM)nStyleNumber, (LPARAM)fPointSize);
	}
	void SetStyleBold(SHORT nStyleNumber, BOOL bBold)
	{
		SendMessage(SCI_STYLESETBOLD, (WPARAM)nStyleNumber, (LPARAM)bBold);
	}
	void SetStyleItalic(SHORT nStyleNumber, BOOL bItalic)
	{
		SendMessage(SCI_STYLESETITALIC, (WPARAM)nStyleNumber, (LPARAM)bItalic);
	}
	void SetStyleUnderline(SHORT nStyleNumber, BOOL bUnderline)
	{
		SendMessage(SCI_STYLESETUNDERLINE, (WPARAM)nStyleNumber, (LPARAM)bUnderline);
	}
	void SetStyleForeground(SHORT nStyleNumber, COLORREF color)
	{
		SendMessage(SCI_STYLESETFORE, (WPARAM)nStyleNumber, (LPARAM)color);
	}
	void SetStyleBackground(SHORT nStyleNumber, COLORREF color)
	{
		SendMessage(SCI_STYLESETBACK, (WPARAM)nStyleNumber, (LPARAM)color);
	}
	void SetStyleEOLFilled(SHORT nStyleNumber, BOOL bEOLFilled)
	{
		SendMessage(SCI_STYLESETEOLFILLED, (WPARAM)nStyleNumber, (LPARAM)bEolFilled);
	}
	void SetStyleCharacterSet(SHORT nStyleNumber, int nCharSet)
	{
		SendMessage(SCI_STYLESETCHARACTERSET, (WPARAM)nStyleNumber, (LPARAM)nCharSet);
	}
	void StyleCase(SHORT nStyleNumber, SHORT nCaseMode)
	{
		SendMessage(SCI_STYLESETCASE, (WPARAM)nStyleNumber, (LPARAM)nCaseMode);
	}
	void StyleVisible(SHORT nStyleNumber, BOOL bVisible)
	{
		SendMessage(SCI_STYLESETVISIBLE, (WPARAM)nStyleNumber, (LPARAM)bVisible);
	}
	void StyleChangeable(SHORT nStyleNumber, BOOL bChangeable)
	{
		SendMessage(SCI_STYLESETCHANGEABLE, (WPARAM)nStyleNumber, (LPARAM)bChangeable);
	}
	void StyleHotspot(SHORT nStyleNumber, BOOL bHotspot)
	{
		SendMessage(SCI_STYLESETHOTSPOT, (WPARAM)nStyleNumber, (LPARAM)bHotspot);
	}

// Caret, selection, and hotspot styles
public:
	void SetSelectionForeground(COLORREF color, BOOL bUseSetting)
	{
		SendMessage(SCI_SETSELFORE, (WPARAM)bUseSetting, (LPARAM)color);
	}
	void SetSelectionBackground(COLORREF color, BOOL bUseSetting)
	{
		SendMessage(SCI_SETSELBACK, (WPARAM)bUseSetting, (LPARAM)color);
	}
	void SetCaretForeground(COLORREF color)
	{
		SendMessage(SCI_SETCARETFORE, (WPARAM)color, 0L);
	}
	COLORREF GetCaretForeground() const
	{
		return (COLORREF)SendMessage(SCI_GETCARETFORE, 0, 0L);
	}
	void SetCaretLineVisible(BOOL bShow)
	{
		SendMessage(SCI_SETCARETLINEVISIBLE, (WPARAM)bShow, 0L);
	}
	BOOL GetCaretLineVisible() const
	{
		return (BOOL)SendMessage(SCI_GETCARETLINEVISIBLE, 0, 0L);
	}
	void SetCaretLineBackground(COLORREF color)
	{
		SendMessage(SCI_SETCARETLINEBACK, (WPARAM)color, 0L);
	}
	COLORREF GetCaretLineBackground() const
	{
		return (COLORREF)SendMessage(SCI_GETCARETLINEBACK, 0, 0L);
	}
	void SetCaretPeriod(LONG lMilliseconds)
	{
		SendMessage(SCI_SETCARETPERIOD, (WPARAM)lMilliseconds, 0L);
	}
	LONG GetCaretPeriod() const
	{
		return (LONG)SendMessage(SCI_GETCARETPERIOD, 0, 0L);
	}
	void SetCaretWidth(int nWidth)
	{
		SendMessage(SCI_SETCARETWIDTH, (WPARAM)nWidth, 0L);
	}
	int GetCaretWidth() const
	{
		return (int)SendMessage(SCI_GETCARETWIDTH, 0, 0L);
	}
	void SetHotspotForeground(COLORREF color, BOOL bUseSetting)
	{
		SendMessage(SCI_SETHOTSPOTACTIVEFORE, (WPARAM)bUseSetting, (LPARAM)color);
	}
	void SetHotspotBackground(COLORREF color, BOOL bUseSetting)
	{
		SendMessage(SCI_SETHOTSPOTACTIVEBACK, (WPARAM)bUseSetting, (LPARAM)color);
	}
	void SetHotspotActiveUnderline(BOOL bShow)
	{
		SendMessage(SCI_SETHOTSPOTACTIVEUNDERLINE, (WPARAM)bShow, 0L);
	}
	void SetHotspotSingleLine(BOOL bShow)
	{
		SendMessage(SCI_SETHOTSPOTSINGLELINE, (WPARAM)bShow, 0L);
	}
	void SetControlCharSymbol(int nSymbol)
	{
		SendMessage(SCI_SETCONTROLCHARSYMBOL, (WPARAM)nWidth, 0L);
	}
	int GetControlCharSymbol() const
	{
		return (int)SendMessage(SCI_GETCONTROLCHARSYMBOL, 0, 0L);
	}

// Margins
public:
	void SetMarginType(SHORT nMargin, int nType)
	{
		SendMessage(SCI_SETMARGINTYPEN, (WPARAM)nMargin, (LPARAM)nType);
	}
	int GetMarginType(SHORT nMargin) const
	{
		return (int)SendMessage(SCI_GETMARGINTYPEN, (WPARAM)nMargin, 0L);
	}
	void SetMarginWidth(SHORT nMargin, int nWidth)
	{
		SendMessage(SCI_SETMARGINWIDTHN, (WPARAM)nMargin, (LPARAM)nWidth);
	}
	int GetMarginWidth(SHORT nMargin) const
	{
		return (int)SendMessage(SCI_GETMARGINWIDTHN, (WPARAM)nMargin, 0L);
	}
	void SetMarginMask(SHORT nMargin, DWORD dwMask)
	{
		SendMessage(SCI_SETMARGINMASKN, (WPARAM)nMargin, (LPARAM)dwMask);
	}
	DWORD GetMarginMask(SHORT nMargin) const
	{
		return (DWORD)SendMessage(SCI_GETMARGINMASKN, (WPARAM)nMargin, 0L);
	}
	void SetMarginSensitivity(SHORT nMargin, BOOL bSensitive)
	{
		SendMessage(SCI_SETMARGINSENSITIVEN, (WPARAM)nMargin, (LPARAM)bSensitive);
	}
	BOOL GetMarginSensitivity(SHORT nMargin) const
	{
		return (BOOL)SendMessage(SCI_GETMARGINSENSITIVEN, (WPARAM)nMargin, 0L);
	}
	void SetLeftMarginWidth(int nWidth)
	{
		SendMessage(SCI_SETMARGINLEFT, 0, (LPARAM)nWidth);
	}
	int GetLeftMarginWidth() const
	{
		return (int)SendMessage(SCI_GETMARGINLEFT, 0, 0L);
	}
	void SetRightMarginWidth(int nWidth)
	{
		SendMessage(SCI_SETMARGINRIGHT, 0, (LPARAM)nWidth);
	}
	int GetRightMarginWidth() const
	{
		return (int)SendMessage(SCI_GETMARGINRIGHT, 0, 0L);
	}
	void SetFoldMarginColor(COLORREF color, BOOL bUseSetting)
	{
		SendMessage(SCI_SETFOLDMARGINCOLOUR, (WPARAM)bUseSetting, (LPARAM)color);
	}
	void SetFoldMarginHighlight(COLORREF color, BOOL bUseSetting)
	{
		SendMessage(SCI_SETFOLDMARGINHICOLOUR, (WPARAM)bUseSetting, (LPARAM)color);
	}

// Other settings
public:
	void SetUsePalette(BOOL bUsePalette)
	{
		SendMessage(SCI_SETUSEPALETTE, (WPARAM)bUsePalette, 0L);
	}
	BOOL GetUsePallete() const
	{
		return (BOOL)SendMessage(SCI_GETUSEPALETTE, 0, 0L);
	}
	void SetBufferedDraw(BOOL bIsBuffered)
	{
		SendMessage(SCI_SETBUFFEREDDRAW, (WPARAM)bIsBuffered, 0L);
	}
	BOOL GetBufferedDraw() const
	{
		return (BOOL)SendMessage(SCI_GETBUFFEREDDRAW, 0, 0L);
	}
	void SetPhasedDraw(BOOL bTwoPhase)
	{
		SendMessage(SCI_SETTWOPHASEDRAW, (WPARAM)bTwoPhase, 0L);
	}
	BOOL GetPhasedDraw() const
	{
		return (BOOL)SendMessage(SCI_GETTWOPHASEDRAW, 0, 0L);
	}
	void SetCodePage(LONG lCodePage)
	{
		SendMessage(SCI_SETCODEPAGE, (WPARAM)lCodePage, 0L);
	}
	LONG GetCodePage() const
	{
		return (LONG)SendMessage(SCI_GETCODEPAGE, 0, 0L);
	}
	void SetWordChars(LPCSTR pszCharacterSet)
	{
		SendMessage(SCI_SETWORDCHARS, 0, (LPARAM)pszCharacterSet);
	}
	void SetWhitespaceChars(LPCSTR pszCharacterSet)
	{
		SendMessage(SCI_SETWHITESPACECHARS, 0, (LPARAM)pszCharacterSet);
	}
	void UseDefaultChars()
	{
		SendMessage(SCI_SETCHARSDEFAULT, 0, 0L);
	}
	void GrabFocus()
	{
		SendMessage(SCI_GRABFOCUS, 0, 0L);
	}
	void SetFocus(BOOL bFocus)
	{
		SendMessage(SCI_SETFOCUS, (WPARAM)bFocus, 0L);
	}
	BOOL GetFocus() const
	{
		return (BOOL)SendMessage(SCI_GETFOCUS, 0, 0L);
	}

// Brace highlighting
public:
	void BraceHighlight(int nPosition1, int nPosition2)
	{
		SendMessage(SCI_BRACEHIGHLIGHT, (WPARAM)nPosition1, (LPARAM)nPosition2);
	}
	void BraceBadlight(int nPosition)
	{
		SendMessage(SCI_BRACEBADLIGHT, (WPARAM)nPosition, 0L);
	}
	int BraceMatch(int nPosition, int nMaxReStyle)
	{
		return (int)SendMessage(SCI_BRACEMATCH, (WPARAM)nPosition, (LPARAM)nMaxReStyle);
	}

// Tabs and Indentation Guides
public:
	void SetTabWidth(int nWidthInChars)
	{
		SendMessage(SCI_SETTABWIDTH, (WPARAM)nLine, 0L);
	}
	int GetTabWidth() const
	{
		return (int)SendMessage(SCI_GETTABWIDTH, 0, 0L);
	}
	void SetUseTabs(BOOL bUseTabs)
	{
		SendMessage(SCI_SETUSETABS, (WPARAM)bUseTabs, 0L);
	}
	BOOL GetUseTabs() const
	{
		return (BOOL)SendMessage(SCI_GETUSETABS, 0, 0L);
	}
	void SetIndent(int nWidthInChars)
	{
		SendMessage(SCI_SETINDENT, (WPARAM)nLine, 0L);
	}
	int GetIndent() const
	{
		return (int)SendMessage(SCI_GETINDENT, 0, 0L);
	}
	void SetTabIndents(BOOL bTabIndents)
	{
		SendMessage(SCI_SETTABINDENTS, (WPARAM)bTabIndents, 0L);
	}
	BOOL GetTabIndents() const
	{
		return (BOOL)SendMessage(SCI_GETTABINDENTS, 0, 0L);
	}
	void SetBackspaceUnindents(BOOL bUnindents)
	{
		SendMessage(SCI_SETBACKSPACEUNINDENTS, (WPARAM)bUnindents, 0L);
	}
	BOOL GetBackspaceUnindents() const
	{
		return (BOOL)SendMessage(SCI_GETBACKSPACEUNINDENTS, 0, 0L);
	}
	void SetLineIndentation(int nLine, int nIndentation)
	{
		SendMessage(SCI_SETLINEINDENTATION, (WPARAM)nLine, (LPARAM)nIndentation);
	}
	int GetLineIndentation(int nLine) const
	{
		return (int)SendMessage(SCI_GETLINEINDENTATION, (WPARAM)nLine, 0L);
	}
	int GetLineIndentPosition(int nLine) const
	{
		return (int)SendMessage(SCI_GETLINEINDENTPOSITION, (WPARAM)nLine, 0L);
	}
	void SetIndentationGuides(BOOL bShow)
	{
		SendMessage(SCI_SETINDENTATIONGUIDES, (WPARAM)bShow, 0L);
	}
	BOOL GetIndentationGuides() const
	{
		return (BOOL)SendMessage(SCI_SETINDENTATIONGUIDES, 0, 0L);
	}
	void SetHighlightGuide(int nColumn)
	{
		SendMessage(SCI_SETHIGHLIGHTGUIDE, (WPARAM)nColumn, 0L);
	}
	BOOL GetHighlightGuide() const
	{
		return (BOOL)SendMessage(SCI_GETHIGHLIGHTGUIDE, 0, 0L);
	}

// Markers
public:
	void MarkerDefine(SHORT nMarkerNumber, int nMarkerSymbols)
	{
		SendMessage(SCI_MARKERDEFINE, (WPARAM)nMarkerNumber, (LPARAM)nMarkerSymbols);
	}
	void MarkerDefinePixmap(SHORT nMarkerNumber, LPCSTR pszData)
	{
		SendMessage(SCI_MARKERDEFINEPIXMAP, (WPARAM)nMarkerNumber, (LPARAM)pszData);
	}
	void SetMarkerForeground(SHORT nMarkerNumber, COLORREF color)
	{
		SendMessage(SCI_MARKERSETFORE, (WPARAM)nMarkerNumber, (LPARAM)color);
	}
	void SetMarkerBackground(SHORT nMarkerNumber, COLORREF color)
	{
		SendMessage(SCI_MARKERSETBACK, (WPARAM)nMarkerNumber, (LPARAM)color);
	}
	void MarkerAdd(int nLine, SHORT nMarkerNumber)
	{
		SendMessage(SCI_MARKERSETBACK, (WPARAM)nLine, (LPARAM)nMarkerNumber);
	}
	void MarkerDelete(int nLine, SHORT nMarkerNumber)
	{
		SendMessage(SCI_MARKERDELETE, (WPARAM)nLine, (LPARAM)nMarkerNumber);
	}
	void MarkerDeleteAll(SHORT nMarkerNumber)
	{
		SendMessage(SCI_MARKERDELETEALL, (WPARAM)nLine, 0L);
	}
	SHORT GetMarker(int nLine)
	{
		return (SHORT)SendMessage(SCI_MARKERGET, (WPARAM)nLine, 0L);
	}
	SHORT GetMarkerNext(int nLineStart, SHORT nMarkerNumber)
	{
		return (SHORT)SendMessage(SCI_MARKERNEXT, (WPARAM)nLineStart, (LPARAM)nMarkerNumber);
	}
	SHORT GetMarkerPrevious(int nLineStart, SHORT nMarkerNumber)
	{
		return (SHORT)SendMessage(SCI_MARKERPREVIOUS, (WPARAM)nLineStart, (LPARAM)nMarkerNumber);
	}
	int MarkerLineFromHandle(DWORD dwHandle) const
	{
		return (int)SendMessage(SCI_MARKERLINEFROMHANDLE, (WPARAM)dwHandle, 0L);
	}
	void MarkerDeleteHandle(DWORD dwHandle)
	{
		SendMessage(SCI_MARKERDELETEHANDLE, (WPARAM)dwHandle, 0L);
	}

// Indicators
public:
	void SetIndicatorStyle(SHORT nIndicatorNumber, DWORD dwStyle)
	{
		SendMessage(SCI_INDICSETSTYLE, (WPARAM)nIndicatorNumber, (LPARAM)dwStyle);
	}
	DWORD GetIndicatorStyle(SHORT nIndicatorNumber) const
	{
		return (DWORD)SendMessage(SCI_INDICGETSTYLE, (WPARAM)nIndicatorNumber, 0L);
	}
	void SetIndicatorForeground(SHORT nIndicatorNumber, COLORREF color)
	{
		SendMessage(SCI_INDICSETFORE, (WPARAM)nIndicatorNumber, (LPARAM)color);
	}
	COLORREF GetIndicatorForeground(SHORT nIndicatorNumber) const
	{
		return (COLORREF)SendMessage(SCI_INDICSETFORE, (WPARAM)nIndicatorNumber, 0L);
	}

// Autocompletion
public:
	void AutoCompleteShow(int nLenghtEntered, LPCSTR pszList)
	{
		SendMessage(SCI_AUTOCSHOW, (WPARAM)nLenghtEntered, (LPARAM)pszList);
	}
	void AutoCompleteCancel()
	{
		SendMessage(SCI_AUTOCCANCEL, 0, 0L);
	}
	void AutoCompleteActivate()
	{
		SendMessage(SCI_AUTOCACTIVE, 0, 0L);
	}
	int AutoCompleteStartPosition() const
	{
		return (int)SendMessage(SCI_AUTOCPOSSTART, 0, 0L);
	}
	void AutoCompleteComplete()
	{
		SendMessage(SCI_AUTOCCOMPLETE, 0, 0L);
	}
	void AutoCompleteStops(LPCSTR pszChars)
	{
		SendMessage(SCI_AUTOCSTOPS, 0, (LPARAM)pszChars);
	}
	void AutoCompleteSeparator(CHAR chSeparator)
	{
		SendMessage(SCI_AUTOCSETSEPARATOR, (WPARAM)chSeparator, 0L);
	}
	CHAR AutoCompleteSeparator() const
	{
		return (CHAR)SendMessage(SCI_AUTOCGETSEPARATOR, 0, 0L);
	}
	void AutoCompleteSelect(LPCSTR pszSelection)
	{
		SendMessage(SCI_AUTOCSELECT, 0, (LPARAM)pszSelection);
	}
	int AutoCompleteGetCurrent() const
	{
		return (int)SendMessage(SCI_AUTOCGETCURRENT, 0, 0L);
	}
	void AutoCompleteCancelAtStart(BOOL bCancel)
	{
		SendMessage(SCI_AUTOCSETCANCELATSTART, (WPARAM)bCancel, 0L);
	}
	BOOL AutoCompleteCancelAtStart() const
	{
		return (BOOL)SendMessage(SCI_AUTOCSETCANCELATSTART, 0, 0L);
	}
	void AutoCompleteFillups(LPCSTR pszChars)
	{
		SendMessage(SCI_AUTOCSETFILLUPS, 0, (LPARAM)pszChars);
	}
	void AutoCompleteChooseSingle(BOOL bChooseSingle)
	{
		SendMessage(SCI_AUTOCSETCHOOSESINGLE, (WPARAM)bChooseSingle, 0L);
	}
	BOOL AutoCompleteChooseSingle() const
	{
		return (BOOL)SendMessage(SCI_AUTOCGETCHOOSESINGLE, 0, 0L);
	}
	void AutoCompleteIgnoreCase(BOOL bChooseSingle)
	{
		SendMessage(SCI_AUTOCSETIGNORECASE, (WPARAM)bChooseSingle, 0L);
	}
	BOOL AutoCompleteIgnoreCase() const
	{
		return (BOOL)SendMessage(SCI_AUTOCGETIGNORECASE, 0, 0L);
	}
	void AutoCompleteAutoHide(BOOL bAutoHide)
	{
		SendMessage(SCI_AUTOCSETAUTOHIDE, (WPARAM)bAutoHide, 0L);
	}
	BOOL AutoCompleteAutoHide() const
	{
		return (BOOL)SendMessage(SCI_AUTOCGETAUTOHIDE, 0, 0L);
	}
	void AutoCompleteDropRestOfWord(BOOL bDropRestOfWord)
	{
		SendMessage(SCI_AUTOCSETDROPRESTOFWORD, (WPARAM)bDropRestOfWord, 0L);
	}
	BOOL AutoCompleteDropRestOfWord() const
	{
		return (BOOL)SendMessage(SCI_AUTOCGETDROPRESTOFWORD, 0, 0L);
	}
	void AutoCompleteRegisterImage(int nType, LPCSTR pszData)
	{
		SendMessage(SCI_REGISTERIMAGE, (WPARAM)nType, (LPARAM)pszData);
	}
	void AutoCompleteClearImages()
	{
		SendMessage(SCI_CLEARREGISTEREDIMAGES, 0, 0L);
	}
	void AutoCompleteTypeSeparator(CHAR chSeparator)
	{
		SendMessage(SCI_AUTOCSETTYPESEPARATOR, (WPARAM)chSeparator, 0L);
	}
	CHAR AutoCompleteTypeSeparator() const
	{
		return (CHAR)SendMessage(SCI_AUTOCGETTYPESEPARATOR, 0, 0L);
	}

// User lists
public:
	void UserListShow(int nListType, LPCSTR pszList)
	{
		SendMessage(SCI_USERLISTSHOW, (WPARAM)nListType, (LPARAM)pszList);
	}

// Call tips
public:
	void CallTipShow(int nPosStart, LPCSTR pszDefinition)
	{
		SendMessage(SCI_CALLTIPSHOW, (WPARAM)nPosStart, (LPARAM)pszDefinition);
	}
	void CallTipCancel()
	{
		SendMessage(SCI_CALLTIPCANCEL, 0, 0L);
	}
	void CallTipActivate()
	{
		SendMessage(SCI_CALLTIPACTIVE, 0, 0L);
	}
	int CallTipStartPosition()
	{
		return (int)SendMessage(SCI_CALLTIPPOSSTART, 0, 0L);
	}
	void CallTipHighlight(int nPosStart, int nPosEnd)
	{
		SendMessage(SCI_CALLTIPSETHLT, (WPARAM)nPosStart, (LPARAM)nPosEnd);
	}
	void SetCallTipBackground(COLORREF color)
	{
		SendMessage(SCI_CALLTIPSETBACK, (WPARAM)color, 0L);
	}
	void SetCallTipForeground(COLORREF color)
	{
		SendMessage(SCI_CALLTIPSETFORE, (WPARAM)color, 0L);
	}
	void SetCallTipHighlight(COLORREF color)
	{
		SendMessage(SCI_CALLTIPSETFOREHLT, (WPARAM)color, 0L);
	}

// Key bindings
public:
	void AssignCmdKey(int nKeyDefinition, int nCommand)
	{
		SendMessage(SCI_ASSIGNCMDKEY, (WPARAM)nKeyDefinition, (LPARAM)nCommand);
	}
	void ClearCmdKey(int nKeyDefinition)
	{
		SendMessage(SCI_CLEARCMDKEY, (WPARAM)nKeyDefinition, 0L);
	}
	void ClearCmdKey()
	{
		SendMessage(SCI_CLEARALLCMDKEYS, 0, 0L);
	}

// Popup edit menu
public:
	void UsePopup(BOOL bEnablePopup)
	{
		SendMessage(SCI_USEPOPUP, (WPARAM)bEnablePopup, 0L);
	}

// Macro recording
public:
	void StartMacroRecording()
	{
		SendMessage(SCI_STARTRECORD, 0, 0L);
	}
	void StopMacroRecording()
	{
		SendMessage(SCI_STOPRECORD, 0, 0L);
	}

// Printing
public:
	int FormatRange(FORMATRANGE* pfr, BOOL bDraw)
	{
		return (int)SendMessage(SCI_FORMATRANGE, (WPARAM)bDraw, (LPARAM)pfr);
	}
	int FormatRange(HDC hdc, HDC hdcTarget, RECT rc, RECT rcPage, int cpMin, int cpMax, BOOL bDraw)
	{
		FORMATRANGE fr = { hdc, hdcTarget, rc, rcPage, {(LONG)cpMin, (LONG)cpMax} };
		return (int)SendMessage(SCI_FORMATRANGE, (WPARAM)bDraw, (LPARAM)&fr);
	}
	void SetPrintMagnification(int nMagnification)
	{
		SendMessage(SCI_SETPRINTMAGNIFICATION, (WPARAM)nMagnification, 0L);
	}
	int GetPrintMagnification() const
	{
		return (int)SendMessage(SCI_GETPRINTMAGNIFICATION, 0, 0L);
	}
	void SetPrintColorMode(SHORT nMode)
	{
		SendMessage(SCI_SETPRINTCOLOURMODE, (WPARAM)nMode, 0L);
	}
	SHORT GetPrintColorMode() const
	{
		return (SHORT)SendMessage(SCI_GETPRINTCOLOURMODE, 0, 0L);
	}
	void SetPrintWrapMode(SHORT nMode)
	{
		SendMessage(SCI_SETPRINTWRAPMODE, (WPARAM)nMode, 0L);
	}
	SHORT GetPrintWrapMode() const
	{
		return (SHORT)SendMessage(SCI_GETPRINTWRAPMODE, 0, 0L);
	}

// Direct access
public:
	SciFnDirect GetDirectFunction() const
	{
		return (SciFnDirect)SendMessage(SCI_GETDIRECTFUNCTION, 0, 0L);
	}
	LPVOID GetDirectPointer() const
	{
		return (LPVOID)SendMessage(SCI_GETDIRECTPOINTER, 0, 0L);
	}

// Multiple views
public:
	void SetDocument(LPVOID pDocument)
	{
		SendMessage(SCI_SETDOCPOINTER, 0, (LPARAM)pDocument);
	}
	LPVOID GetDocument() const
	{
		return (LPVOID)SendMessage(SCI_GETDOCPOINTER, 0, 0L);
	}
	LPVOID CreateDocument() const
	{
		return (LPVOID)SendMessage(SCI_CREATEDOCUMENT, 0, 0L);
	}
	void DocumentAddRef(LPVOID pDocument)
	{
		SendMessage(SCI_ADDREFDOCUMENT, 0, (LPARAM)pDocument);
	}
	void DocumentRelease(LPVOID pDocument)
	{
		SendMessage(SCI_RELEASEDOCUMENT, 0, (LPARAM)pDocument);
	}

// Folding
public:
	int VisibleFromDocumentLine(int nLine)
	{
		return (int)SendMessage(SCI_VISIBLEFROMDOCLINE, (WPARAM)nLine, 0L);
	}
	int DocumentFromVisibleLine(int nDisplayLine)
	{
		return (int)SendMessage(SCI_DOCLINEFROMVISIBLE, (WPARAM)nDisplayLine, 0L);
	}
	void ShowLines(int nStartLine, int nEndLine)
	{
		SendMessage(SCI_SHOWLINES, (WPARAM)nStartLine, (LPARAM)nEndLine);
	}
	void HideLines(int nStartLine, int nEndLine)
	{
		SendMessage(SCI_HIDELINES, (WPARAM)nStartLine, (LPARAM)nEndLine);
	}
	BOOL IsLineVisible(int nLine) const
	{
		return (BOOL)SendMessage(SCI_GETLINEVISIBLE, (WPARAM)nLine, 0L);
	}
	int SetFoldLevel(int nLine, int nLevel)
	{
		return (int)SendMessage(SCI_SETFOLDLEVEL, (WPARAM)nLine, (LPARAM)nLevel);
	}
	int GetFoldLevel(int nLine) const
	{
		return (int)SendMessage(SCI_SETFOLDLEVEL, (WPARAM)nLine, 0L);
	}
	void SetFoldFlags(DWORD dwFlags)
	{
		SendMessage(SCI_SETFOLDFLAGS, (WPARAM)dwFlags, 0L);
	}
	int GetLastChild(int nLine, int nLevel) const
	{
		return (int)SendMessage(SCI_GETLASTCHILD, (WPARAM)nLine, (LPARAM)nLevel);
	}
	int GetFoldParent(int nLine) const
	{
		return (int)SendMessage(SCI_GETFOLDPARENT, (WPARAM)nLine,0L);
	}
	void SetFoldExpanded(int nLine, BOOL bExpanded)
	{
		SendMessage(SCI_SETFOLDEXPANDED, (WPARAM)nLine, (LPARAM)bExpanded);
	}
	BOOL GetFoldExpanded(int nLine) const
	{
		return (BOOL)SendMessage(SCI_GETFOLDEXPANDED, (WPARAM)nLine, 0L);
	}
	void ToggleFold(int nLine)
	{
		SendMessage(SCI_TOGGLEFOLD, (WPARAM)nLine, 0L);
	}
	void EnsureVisible(int nLine)
	{
		SendMessage(SCI_ENSUREVISIBLE, (WPARAM)nLine, 0L);
	}
	void EnsureVisiblePolicy(int nLine)
	{
		SendMessage(SCI_ENSUREVISIBLEENFORCEPOLICY, (WPARAM)nLine, 0L);
	}

// Line wrapping
public:
	void SetWrapMode(BOOL bWrap)
	{
		SendMessage(SCI_SETWRAPMODE, (WPARAM)(bWrap ? SC_WRAP_WORD : SC_WRAP_NONE), 0L);
	}
	BOOL IsWrapped() const
	{
		return (SendMessage(SCI_GETWRAPMODE, (WPARAM)nLine, 0L) == SC_WRAP_WORD);
	}
	void SetWrapVisualFlags(DWORD dwFlags)
	{
		SendMessage(SCI_SETWRAPVISUALFLAGS, (WPARAM)dwFlags, 0L);
	}
	DWORD GetWrapVisualFlags() const
	{
		return (DWORD)SendMessage(SCI_GETWRAPVISUALFLAGS, 0, 0L);
	}
	void SetWrapStartIndent(int nIndent)
	{
		SendMessage(SCI_SETWRAPSTARTINDENT, (WPARAM)nIndent, 0L);
	}
	int GetWrapStartIndent() const
	{
		return (int)SendMessage(SCI_GETWRAPSTARTINDENT, 0, 0L);
	}
	void SetLayoutCache(SHORT nMode)
	{
		SendMessage(SCI_SETLAYOUTCACHE, (WPARAM)nMode, 0L);
	}
	SHORT GetLayoutCache() const
	{
		return (SHORT)SendMessage(SCI_GETLAYOUTCACHE, 0, 0L);
	}
	void LinesSplit(int nMaxWidth)
	{
		SendMessage(SCI_LINESSPLIT, (WPARAM)nMaxWidth, 0L);
	}
	void LinesJoin()
	{
		SendMessage(SCI_LINESJOIN, 0, 0L);
	}

// Zooming
public:
	void ZoomIn()
	{
		SendMessage(SCI_ZOOMIN, 0, 0L);
	}
	void ZoomOut()
	{
		SendMessage(SCI_ZOOMOUT, 0, 0L);
	}
	void SetZoom(int nZoomInPoints)
	{
		SendMessage(SCI_SETZOOM, (WPARAM)nZoomInPoints, 0L);
	}
	int GetZoom() const
	{
		return (int)SendMessage(SCI_GETZOOM, 0, 0L);
	}

// Long lines
public:
	void SetEdgeMode(SHORT nMode)
	{
		SendMessage(SCI_SETEDGEMODE, (WPARAM)nMode, 0L);
	}
	SHORT GetEdgeMode() const
	{
		return (SHORT)SendMessage(SCI_GETEDGEMODE, 0, 0L);
	}
	void SetEdgeColumn(int nColumn)
	{
		SendMessage(SCI_SETEDGECOLUMN, (WPARAM)nColumn, 0L);
	}
	int GetEdgeColumn() const
	{
		return (int)SendMessage(SCI_GETEDGECOLUMN, 0, 0L);
	}
	void SetEdgeColor(COLORREF color)
	{
		SendMessage(SCI_SETEDGECOLOUR, (WPARAM)color, 0L);
	}
	COLORREF GetEdgeColor() const
	{
		return (COLORREF)SendMessage(SCI_GETEDGECOLOUR, 0, 0L);
	}

// Lexer
public:
	void SetLexer(UINT nLexer)
	{
		SendMessage(SCI_SETLEXER, (WPARAM)nLexer, 0L);
	}
	UINT GetLexer() const
	{
		return (UINT)SendMessage(SCI_GETLEXER, 0, 0L);
	}
	void SetLexerLanguage(LPSTR pszName)
	{
		SendMessage(SCI_SETLEXERLANGUAGE, 0, (LPARAM)pszName);
	}
	void LoadLexerLibrary(LPSTR pszPath)
	{
		SendMessage(SCI_LOADLEXERLIBRARY, 0, (LPARAM)pszPath);
	}
	void Colourise(int nStartChar, int nEndChar)
	{
		SendMessage(SCI_COLOURISE, (WPARAM)nStartChar, (LPARAM)nEndChar);
	}
	void SetProperty(LPCSTR pszKey, LPCSTR pszValue)
	{
		SendMessage(SCI_SETPROPERTY, (WPARAM)pszKey, (LPARAM)pszValue);
	}
	void SetKeyWords(int nKeyWordSet, LPCSTR pszKeyWords)
	{
		SendMessage(SCI_SETKEYWORDS, (WPARAM)nKeyWordSet, (LPARAM)pszKeyWords);
	}
};

typedef CScintillaCtrlT<ATL::CWindow> CScintillaCtrl;

#ifndef STATIC_BUILD
#define SCINTILLA_STYLENEEDED_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_STYLENEEDED) \
	{ \
		SetMsgHandled(TRUE); \
		int startPos = (int)::SendMessage(((LPNMHDR)lParam)->hwndFrom, SCI_GETENDSTYLED, 0, 0L); \
		func(startPos, ((SCNotification*)lParam)->position); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#else
#define SCINTILLA_STYLENEEDED_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_STYLENEEDED) \
	{ \
		SetMsgHandled(TRUE); \
		int startPos = (int)Scintilla_DirectFunction(((LPNMHDR)lParam)->hwndFrom, SCI_GETENDSTYLED, 0, 0L); \
		func(startPos, ((SCNotification*)lParam)->position); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#endif
#define SCINTILLA_CHARADDED_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_CHARADDED) \
	{ \
		SetMsgHandled(TRUE); \
		func(((SCNotification*)lParam)->ch); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_SAVEPOINTREACHED_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_SAVEPOINTREACHED) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_SAVEPOINTLEFT_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_SAVEPOINTLEFT) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_MODIFYATTEMPTRO_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_MODIFYATTEMPTRO) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_DOUBLECLICK_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_DOUBLECLICK) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_UPDATEUI_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_UPDATEUI) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_MODIFIED_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_MODIFIED) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->position, pSCN->modificationType, pSCN->text, pSCN->length, \
			pSCN->linesAdded, pSCN->line, pSCN->foldLevelNow, pSCN->foldLevelPrev); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_MACRORECORD_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_MACRORECORD) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->message, pSCN->wParam, pSCN->lParam); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_MARGINCLICK_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_MARGINCLICK) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->position, pSCN->margin, pSCN->modifiers); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_NEEDSHOWN_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_NEEDSHOWN) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->position, pSCN->length); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_PAINTED_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_PAINTED) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_USERLISTSELECTION_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_USERLISTSELECTION) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->listType, pSCN->text); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_DWELLSTART_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_DWELLSTART) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->position, pSCN->x, pSCN->y); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_DWELLEND_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_DWELLEND) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->position, pSCN->x, pSCN->y); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_ZOOM_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_ZOOM) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_HOTSPOTCLICK_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_HOTSPOTCLICK) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->position, pSCN->modifiers); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_HOTSPOTDOUBLECLICK_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_HOTSPOTDOUBLECLICK) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->position, pSCN->modifiers); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_CALLTIPCLICK_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_CALLTIPCLICK) \
	{ \
		SetMsgHandled(TRUE); \
		func(((SCNotification*)lParam)->position); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
#define SCINTILLA_AUTOCSELECTION_HANDLER(func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR)lParam)->code == SCN_AUTOCSELECTION) \
	{ \
		SetMsgHandled(TRUE); \
		SCNotification* pSCN = (SCNotification*)lParam; \
		func(pSCN->listType, pSCN->text); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}

///////////////////////////////////////////////////////////////////////////////
// CScintillaEditCommands - message handlers for standard EDIT commands

// Chain to CScintillaEditCommands message map. Your class must also derive from CEdit.
// Example:
// class CMyEdit : public CWindowImpl<CMyEdit, CScintillaCtrl>,
//                 public CScintillaEditCommands<CMyEdit>
// {
// public:
//      BEGIN_MSG_MAP(CMyEdit)
//              // your handlers...
//              CHAIN_MSG_MAP_ALT(CScintillaEditCommands<CMyEdit>, 1)
//      END_MSG_MAP()
//      // other stuff...
// };

template <class T>
class CScintillaEditCommands
{
public:
	BEGIN_MSG_MAP(CScintillaEditCommands< T >)
	ALT_MSG_MAP(1)
		COMMAND_ID_HANDLER(ID_EDIT_CLEAR, OnEditClear)
		COMMAND_ID_HANDLER(ID_EDIT_CLEAR_ALL, OnEditClearAll)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectAll)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
		COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditRedo)
	END_MSG_MAP()

	LRESULT OnEditClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->Clear();
		return 0;
	}

	LRESULT OnEditClearAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->ClearAll();
		return 0;
	}

	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->Copy();
		return 0;
	}

	LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->Cut();
		return 0;
	}

	LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->Paste();
		return 0;
	}

	LRESULT OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->SetSelection(0, -1);
		return 0;
	}

	LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->Undo();
		return 0;
	}

	LRESULT OnEditRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->Redo();
		return 0;
	}

// State (update UI) helpers
public:
	BOOL CanCut() const
	{ return HasSelection() && !IsReadOnly(); }

	BOOL CanCopy() const
	{ return HasSelection(); }

	BOOL CanClear() const
	{ return HasSelection() && !IsReadOnly(); }

	BOOL CanSelectAll() const
	{ return HasText(); }

	BOOL CanFind() const
	{ return HasText(); }

	BOOL CanRepeat() const
	{ return HasText(); }

	BOOL CanReplace() const
	{ return HasText() && !IsReadOnly(); }

	BOOL CanClearAll() const
	{ return HasText() && !IsReadOnly(); }

// Implementation
public:
	BOOL HasSelection() const
	{
		const T* pT = static_cast<const T*>(this);
		int nMin, nMax;
		pT->SendMessage(EM_GETSEL, (WPARAM)&nMin, (LPARAM)&nMax);
		return (nMin != nMax);
	}

	BOOL HasText() const
	{
		const T* pT = static_cast<const T*>(this);
		return (pT->GetTextLength() > 0);
	}
	BOOL IsReadOnly() const
	{
		const T* pT = static_cast<const T*>(this); //pT;
		return pT->GetReadOnly();
	}
};

}; //namespace WETLA

#endif // __WETLA_SCILEX_H__
