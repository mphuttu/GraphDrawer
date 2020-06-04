
// GraphDrawerView.h : interface of the CGraphDrawerView class
//

#pragma once
#include "atltypes.h"


class CGraphDrawerView : public CScrollView
{
protected: // create from serialization only
	CGraphDrawerView();
	DECLARE_DYNCREATE(CGraphDrawerView)

// Attributes
public:
	CGraphDrawerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // Called for the first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CGraphDrawerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
private:
	CRect m_rcPrintRect;
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
//	CFont m_PrintFont;
protected:
	// Printing and print preview
	enum {ELEMENTS_PER_PAGE = 4, TOP_MARGIN = 100, 
		BOTTOM_MARGIN = 100, LEFT_MARGIN = 200 };

	CFont m_PrintFont;
	void PrintPageHeader(CDC* pDC, CPrintInfo* pInfo, CRect m_rcPrintRect);
	void PrintPageFooter(CDC* pDC, CPrintInfo* pInfo, CRect m_rcPrintRect);
	void PrintPageBody(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in GraphDrawerView.cpp
inline CGraphDrawerDoc* CGraphDrawerView::GetDocument() const
   { return reinterpret_cast<CGraphDrawerDoc*>(m_pDocument); }
#endif

