
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "DrawFunctionsDialog.h"
#include "DrawGeometricFiguresDialog.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	BOOL AreDrawFuncVisible()  { return m_bDrawFuncVisible; }
	void SetDrawFuncVisible ( BOOL bSet ) {
		m_bDrawFuncVisible = bSet;
		if (::IsWindow(m_wndDrawFunctionsDialog.m_hWnd))
			m_wndDrawFunctionsDialog.ShowWindow(bSet ? SW_SHOW : SW_HIDE);
	}

	BOOL AreGeoFiguresVisible() { return m_bGeoFiguresVisible; }
	void SetGeoFiguresVisible(BOOL bSet) {
		m_bGeoFiguresVisible = bSet;
		if (::IsWindow(m_wndDrawGeometricFiguresDialog.m_hWnd))
			m_wndDrawGeometricFiguresDialog.ShowWindow(bSet ? SW_SHOW : SW_HIDE);
	}

	// Called by the geo dialog to refresh after structural changes.
	CDrawGeometricFiguresDialog& GetGeoDialog() { return m_wndDrawGeometricFiguresDialog; }

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateCR(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	CDrawFunctionsDialog          m_wndDrawFunctionsDialog;
	BOOL                          m_bDrawFuncVisible;

	CDrawGeometricFiguresDialog   m_wndDrawGeometricFiguresDialog;
	BOOL                          m_bGeoFiguresVisible;

public:
	afx_msg void OnViewDrawfunctions();
	afx_msg void OnUpdateViewDrawfunctions(CCmdUI *pCmdUI);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnHelpContents();
	afx_msg void OnDrawGeometricFigures();
	afx_msg void OnUpdateDrawGeometricFigures(CCmdUI* pCmdUI);
};


