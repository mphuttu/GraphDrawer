
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

	// Create the draw functions dialog window
	m_wndDrawFunctionsDialog.Create(IDD_DRAWFUNCTIONSDIALOG);
	SetDrawFuncVisible(TRUE);
	m_bDrawFuncVisible = TRUE;
	m_wndDrawFunctionsDialog.SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	
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
