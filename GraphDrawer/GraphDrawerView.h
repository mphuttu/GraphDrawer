
// GraphDrawerView.h : interface of the CGraphDrawerView class
//

#pragma once
#include "atltypes.h"
#include "GraphDrawerDoc.h"


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
	afx_msg void OnSize(UINT nType, int cx, int cy);
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
	afx_msg LRESULT OnCustomFunctionReady(WPARAM wParam, LPARAM lParam);
private:
	CRect m_rcPrintRect;
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
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

	// Zoom and pan state
	double m_dViewXMin, m_dViewXMax;
	double m_dViewYMin, m_dViewYMax;

	// Sync view range from doc settings (call after range change).
	void SyncRangeFromDoc();

	// Build a CoordTransform for the current view state.
	CoordTransform BuildCoordTransform(CDC* pDC) const;

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

private:
	BOOL   m_bDragging;
	CPoint m_ptLastMouse;  // last mouse position in device coords
};

#ifndef _DEBUG  // debug version in GraphDrawerView.cpp
inline CGraphDrawerDoc* CGraphDrawerView::GetDocument() const
   { return reinterpret_cast<CGraphDrawerDoc*>(m_pDocument); }
#endif

