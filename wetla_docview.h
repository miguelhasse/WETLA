// WTL Document/View approach
// Author: Gabriel Kniznik (gkniznik@argsoft.com)

#ifndef __WETLA_DOCVIEW_H__
#define	__WETLA_DOCVIEW_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_docview.h requires atlapp.h to be included first
#endif

#define	HINT_UPDATE_ALL_DOCUMENTS	-1
#define	HINT_DOCUMENT_MODIFIED		-2


/////////////////////////////////////////////////////////////////////////////
// Classes in this file
//
// CCreateContext<TDoc, TView>
// CView<TDoc>
// CViewImpl<T, TView>
// CDocument<T, TDoc, TDocTemplate>
// CDocTemplate<TDoc, TView, TFrame, nID>
// CDocManager

namespace WETLA
{

// Forward declarations
class CDocTemplateBase;

// Helper struct for document creation
template <class	TDoc, class TView>
struct CCreateContext
{
	TDoc* m_pCurrentDoc;
	TView* m_pCurrentView;
	CDocTemplateBase* m_pNewDocTemplate;

	CCreateContext()
	{
		memset(this, 0, sizeof(*this));
	}

	// constructor based on a different context handles more than one view per document (splitter windows)	
	template <class TOtherView>
	CCreateContext(CCreateContext<TDoc, TOtherView>* pContext)
	{ 
		m_pCurrentDoc =	pContext->m_pCurrentDoc;
		m_pNewDocTemplate =	pContext->m_pNewDocTemplate;
	}
};

// Abstract class for all the views belonging to a document
template <class	TDoc>
class CView
{
protected:
	TDoc* m_pDocument;

public:
	virtual void OnUpdate(CView<TDoc>* pSender, LPARAM lHint, LPVOID pHint) = 0;

	TDoc* GetDocument()
	{
		return m_pDocument;
	}

	void SetDocument(TDoc* pDoc)
	{
		m_pDocument = pDoc;
	}

	// useful function for updating all docs from a particular template in MDI architectures
	void UpdateAllDocs()
	{
		CDocTemplateBase* pDocTemplate = m_pDocument->GetDocTemplate();
		int ndocs = pDocTemplate->GetDocCount();

		for(int	i = 0; i < ndocs; i++)
		{
			TDoc* pDoc = static_cast<TDoc*>(pDocTemplate->GetDocument(i));
			if (pDoc == NULL) continue;

			pDoc->UpdateAllViews(this, HINT_UPDATE_ALL_DOCUMENTS);
		}
	}
};

// Base class for view implementations
template <class	T , class TView, class TBase = ATL::CWindowImpl<T> >
class ATL_NO_VTABLE CViewImpl : public TBase, public TView
{
public:
	HWND GetParentFrame()
	{
		T* pT =	static_cast<T*>(this);
		HWND hWnd = pT->GetParent();

		while (!(GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_MDICHILD))
			hWnd = ::GetParent(hWnd);

		return hWnd;
	}

	virtual void OnUpdate(TView* pSender, LPARAM lHint, LPVOID pHint)
	{
		// invalidate the entire pane, erase background too
		TBase::Invalidate(TRUE);
	}

	virtual void OnFinalMessage(HWND hWnd)
	{
		m_pDocument->RemoveView(this);
		delete this;
	}
};

// Empty class just for polymorphism
class CDocumentBase
{
	virtual BOOL SaveModified() = 0;
	virtual BOOL OnNewDocument() = 0;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) = 0;
	virtual BOOL OnSaveDocument() = 0;
	virtual void OnCloseDocument() = 0;
};

// The document
template <class	T, class TDoc = CDocumentBase, class TDocTemplate =	CDocTemplateBase>
class ATL_NO_VTABLE CDocument : public TDoc
{
public:
	TDocTemplate* m_pDocTemplate;

protected:
	CSimpleArray<CView<T>*> m_aViews;
	BOOL m_bModified;

public:
	CDocument()
	{
		m_bModified = FALSE;
	}

	void UpdateAllViews(CView<T>* pSender, LPARAM lHint = 0, LPVOID pHint = NULL)
	{
		int nViewCount = m_aViews.GetSize();
		for (int i = 0; i < nViewCount; i++)
		{
			CView<T>* pView = m_aViews[i];
			if (pView != (CView<T>*)pSender)
				pView->OnUpdate(pSender, lHint, pHint);
		}
	}

	CView<T>* AddView(CView<T>* pView)
	{
		pView->SetDocument(static_cast<T*>(this));
		m_aViews.Add(static_cast<CView<T>*>(pView));
		return pView;
	}

	void RemoveView(CView<T>* pView)
	{
		int nViewCount = m_aViews.GetSize();
		for (int i = 0; i < nViewCount; i++)
		{
			if (m_aViews[i] == pView)
			{
				m_aViews.RemoveAt(i);
				pView->SetDocument(NULL);
				break;
			}
		}
	}

	int GetViewCount() const
	{
		return m_aViews.GetSize();
	}

	CView<T>* GetView(const int pos)
	{
		_ASSERTE(pos < m_aViews.GetSize());
		return m_aViews[pos];
	}

	BOOL IsModified()
	{
		return m_bModified;
	}

	void SetModifiedFlag(BOOL bModified = TRUE)
	{
		m_bModified = bModified;
	}

	TDocTemplate* GetDocTemplate()
	{
		return m_pDocTemplate;
	}
};

// Abstract base class for document templates
class CDocTemplateBase
{
public:
	virtual CDocumentBase* GetDocument(int pos) = 0;
	virtual CDocumentBase* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE) = 0;
//	virtual CDocumentBase* OpenDocument(BOOL bNew = FALSE) = 0;
	virtual int GetDocCount() const = 0;

	virtual BOOL SaveAllModified() = 0;
	virtual BOOL OnCmdMsg(HWND hWndCtrl, UINT nID, int nCode, HMENU hMenu) = 0;
};

// The document template
template <class	TDoc, class TView, class TFrame, int nID>
class CDocTemplate : public CDocTemplateBase
{
private:
	CSimpleArray<TDoc*> m_aDocuments;

public:
	HWND m_hWndClient;

public:
	~CDocTemplate()
	{
		int nDocCount = m_aDocuments.GetSize();
		for (int i = 0; i < nDocCount; i++)
			delete	m_aDocuments[i];

		m_aDocuments.RemoveAll();
	}

	TFrame* CreateNewFrame(TDoc* pDoc, TFrame* pOther)
	{
		CCreateContext<TDoc, TView> context;
		context.m_pCurrentDoc =	pDoc;
		context.m_pNewDocTemplate =	this;
		context.m_pCurrentView = new TView;

		pDoc->AddView(context.m_pCurrentView);
		TFrame::GetWndClassInfo().m_uCommonResourceID =	nID;

		TFrame*	pFrame = new TFrame;
		pFrame->CreateEx(m_hWndClient, NULL, NULL, 0, 0, &context);

		return pFrame;
	}

	TDoc* CreateNewDocument()
	{
		TDoc* pDoc = new TDoc;
		AddDocument(pDoc);
		return pDoc;
	}

	virtual CDocumentBase* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible)
	{
		TDoc* pDoc = CreateNewDocument();
		TFrame*	pFrame = CreateNewFrame(pDoc, 0);

		if (lpszPathName)
		{
			if (!pDoc->OnOpenDocument(lpszPathName))
				pFrame->DestroyWindow();
		}
		else
		{
			if (!pDoc->OnNewDocument())
				pFrame->DestroyWindow();
		}

		return pDoc;
	}

	virtual CDocumentBase* GetDocument(int pos)
	{
		return m_aDocuments[pos];
	}

	virtual void AddDocument(TDoc* pDoc)
	{
		pDoc->m_pDocTemplate = this;
		m_aDocuments.Add(pDoc);
	}

	virtual TDoc* RemoveDocument(TDoc* pDoc)
	{
		int nDocCount = m_aDocuments.GetSize();
		for (int i = 0; i < nDocCount; i++)
		{
			if (m_aDocuments[i] == pDoc)
			{
				m_aDocuments.RemoveAt(i);
				break;
			}
		}
		return pDoc;
	}

	virtual int GetDocCount() const
	{
		return m_aDocuments.GetSize();
	}

	virtual BOOL SaveAllModified()
	{
		int nDocCount = m_aDocuments.GetSize();
		for (int i = 0; i < nDocCount; i++)
		{
			if (m_aDocuments[i]->SaveModified() &&
				!m_aDocuments[i]->OnSaveDocument())
				return FALSE;
		}
		return TRUE;
	}

	virtual BOOL OnCmdMsg(HWND hWndCtrl, UINT nID, int nCode, HMENU hMenu)
	{
		return FALSE;
	}
};

// The document manager
class CDocManager : public CMessageFilter
{
protected:
	CSimpleArray<CDocTemplateBase*> m_aTemplates;

public:
	template <class TDocTemplate, class	TMainFrame>
	int AddDocTemplate(TDocTemplate* pDocTemplate, TMainFrame* pMainFrame)
	{
		_ASSERTE(pDocTemplate != NULL);

		pDocTemplate->m_hWndClient = 
			static_cast<TMainFrame*>(pMainFrame)->m_hWndClient;
		CDocTemplateBase* pDocBase = 
			static_cast<CDocTemplateBase*>(pDocTemplate);

		m_aTemplates.Add(pDocBase);
		return m_aTemplates.GetSize() -	1;
	}

	int GetDocTemplateCount() const
	{
		return m_aTemplates.GetSize();
	}

	CDocTemplateBase* GetDocTemplate(const int pos)
	{
		_ASSERTE(pos < m_aTemplates.GetSize());
		return m_aTemplates[pos];
	}

// Message filter operations
	BOOL ConnectMessageLoop(CMessageLoop* pMessageLoop)
	{
		return (pMessageLoop->m_aMsgFilter.Find(this) == -1) ?
			pMessageLoop->m_aMsgFilter.Add(this) : FALSE;
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg == NULL)
			return FALSE;

		HWND hWndCtrl = NULL;
		HMENU hMenu = NULL;
		UINT nID = 0;
		int nCode = 0;

		switch (pMsg->message)
		{
		case WM_COMMAND:

			hWndCtrl = (HWND)pMsg->lParam;
			nID = LOWORD(pMsg->wParam);
			nCode = HIWORD(pMsg->wParam);
			break;

#if defined WINVER >= 0x0500
		case WM_MENUCOMMAND:

			nID = (UINT)pMsg->wParam;
			hMenu = (HMENU)pMsg->lParam;
			break;
#endif
		default:
			return FALSE;
		}

		int nTemplateCount = m_aTemplates.GetSize();
		for (int i = 0; nID && i < nTemplateCount; i++)
		{
			CDocTemplateBase* pTemplate = m_aTemplates[i];
			if (pTemplate == NULL) continue;

			if (pTemplate->OnCmdMsg(hWndCtrl, nID, nCode, hMenu))
				return TRUE;
		}

		if (nID == ID_FILE_NEW)
			return (GetDocTemplate(0)->OpenDocumentFile(NULL) != NULL);

		return FALSE;
	}
};

}; //namespace WETLA

#endif // __WETLA_DOCVIEW_H__