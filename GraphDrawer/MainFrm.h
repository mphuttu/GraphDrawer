
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "DrawFunctionsDialog.h"

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
		if ( bSet ) m_wndDrawFunctionsDialog.ShowWindow(SW_SHOW);
		else 
			m_wndDrawFunctionsDialog.ShowWindow(SW_HIDE);
	}

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

	CDrawFunctionsDialog m_wndDrawFunctionsDialog;
	BOOL m_bDrawFuncVisible;
public:
	afx_msg void OnViewDrawfunctions();
	afx_msg void OnUpdateViewDrawfunctions(CCmdUI *pCmdUI);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
};


