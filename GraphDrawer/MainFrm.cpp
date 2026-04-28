
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GraphDrawer.h"

#include "MainFrm.h"
#include "GraphDrawerDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COORDSIZE, OnUpdateCR)
	ON_COMMAND(ID_VIEW_DRAWFUNCTIONS, &CMainFrame::OnViewDrawfunctions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWFUNCTIONS, &CMainFrame::OnUpdateViewDrawfunctions)
	ON_COMMAND(ID_HELP_CONTENTS, &CMainFrame::OnHelpContents)
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,			// status line indicator
	ID_INDICATOR_COORDSIZE, // show size of coordinate axes
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Create the draw functions dialog window.
	// Position it at a safe offset so it does not cover the menu bar.
	m_wndDrawFunctionsDialog.Create(IDD_DRAWFUNCTIONSDIALOG);
	m_bDrawFuncVisible = TRUE;
	// Place the palette to the right of / below the toolbar area.
	// SWP_NOZORDER keeps it as a regular window (not always-on-top) so the
	// menu remains fully accessible.
	m_wndDrawFunctionsDialog.SetWindowPos(NULL, 10, 200, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	SetDrawFuncVisible(TRUE);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers
void CMainFrame::OnUpdateCR(CCmdUI* pCmdUI)
{
	CGraphDrawerApp* pApp = dynamic_cast<CGraphDrawerApp*>(AfxGetApp() );
	ASSERT_VALID(pApp);

	CDC *dc=GetDC();
	
	// Set mapping mode
	dc->SetMapMode(MM_LOMETRIC);
	CRect rcClient;
	GetClientRect(&rcClient);
	// Convert to logical units
	 dc->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;
	
	int nEndTickY =  -(int) (nHalfHeight*0.99);
	int nEndTickX = (int) ( nHalfWidth*0.99);

	CString strHeights;
	strHeights.Format(_T("CX: %d and CY: %d"), nEndTickX, nEndTickY);
	// AfxMessageBox(strHeights);
	pCmdUI->SetText(strHeights);
}


void CMainFrame::OnViewDrawfunctions()
{
	// TODO: Add your command handler code here
	SetDrawFuncVisible(m_bDrawFuncVisible ? FALSE : TRUE );
}


void CMainFrame::OnUpdateViewDrawfunctions(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	BOOL bEnable = FALSE;
	// CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *>(GetActiveDocument() );

	pCmdUI->Enable(TRUE);

	if ( bEnable)
		pCmdUI->SetCheck(m_bDrawFuncVisible ? 1 : 0 );
}


void CMainFrame::OnHelpContents()
{
	// Build path: <folder containing the exe>\Help\GraphDrawer.chm
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);

	// Trim the exe filename to get the directory
	TCHAR* pLastSlash = _tcsrchr(szPath, _T('\\'));
	if (pLastSlash)
	{
		_tcscpy_s(pLastSlash + 1,
		          MAX_PATH - static_cast<int>(pLastSlash - szPath) - 1,
		          _T("Help\\GraphDrawer.chm"));
	}

	// Open the help file; ShellExecute picks the default CHM viewer.
	HINSTANCE hResult = ShellExecute(
		GetSafeHwnd(), _T("open"), szPath, NULL, NULL, SW_SHOWNORMAL);

	if (reinterpret_cast<INT_PTR>(hResult) <= 32)
	{
		AfxMessageBox(
			_T("Could not open the help file.\n"
			   "Make sure Help\\GraphDrawer.chm exists next to the application."),
			MB_OK | MB_ICONWARNING);
	}
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CFrameWnd::OnActivateApp(bActive, dwThreadID);

	// TODO: Add your message handler code here
	if ( bActive )
	{
		if ( AreDrawFuncVisible () )
			m_wndDrawFunctionsDialog.ShowWindow(SW_SHOW);
	}
	else
	{
		if ( AreDrawFuncVisible() )
			m_wndDrawFunctionsDialog.ShowWindow(SW_HIDE);
	}
}
