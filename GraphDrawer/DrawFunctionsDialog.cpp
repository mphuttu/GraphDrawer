// DrawFunctionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDrawer.h"
#include "DrawFunctionsDialog.h"
#include "afxdialogex.h"

#include "DrawOptionsDialog.h"

#include "MainFrm.h"
#include "GraphDrawerDoc.h"

// CDrawFunctionsDialog dialog

IMPLEMENT_DYNAMIC(CDrawFunctionsDialog, CDialog)

CDrawFunctionsDialog::CDrawFunctionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawFunctionsDialog::IDD, pParent)
	, m_strExpression(_T(""))
	, m_bDrawCustomFunction(FALSE)
	, m_dRangeStart(-20.0)
	, m_dRangeEnd(20.0)
	, m_bDrawSine(FALSE)
	, m_bDrawCosine(FALSE)
	, m_bDrawTan(FALSE)
	, m_bDrawCotan(FALSE)
	, m_bDrawExp(FALSE)
	, m_bDrawLN(FALSE)
	, m_bDrawArcsine(FALSE)
	, m_bDrawArccosine(FALSE)
	, m_bDrawArctan(FALSE)
	, m_bDrawArccotan(FALSE)
	, m_bDrawHyperbolicSine(FALSE)
	, m_bDrawHyperbolicCosine(FALSE)
	, m_bDrawHyperbolicTan(FALSE)
	, m_bDrawHyperbolicCotan(FALSE)
{

}

CDrawFunctionsDialog::~CDrawFunctionsDialog()
{
}

void CDrawFunctionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,  IDC_FUNCEDIT,             m_strExpression);
	DDX_Check(pDX, IDC_DRAWCUSTOMCHECK,      m_bDrawCustomFunction);
	DDX_Text(pDX,  IDC_RANGEEDIT_FROM,       m_dRangeStart);
	DDX_Text(pDX,  IDC_RANGEEDIT_TO,         m_dRangeEnd);
	DDX_Check(pDX, IDC_DRAWSINECHECK,        m_bDrawSine);
	DDX_Check(pDX, IDC_DRAWCOSINECHECK, m_bDrawCosine);
	DDX_Check(pDX, IDC_DRAWTANCHECK, m_bDrawTan);
	DDX_Check(pDX, IDC_DRAWCOTANCHECK, m_bDrawCotan);
	DDX_Check(pDX, IDC_DRAWEXPCHECK, m_bDrawExp);
	DDX_Check(pDX, IDC_DRAWLNCHECK, m_bDrawLN);
	DDX_Check(pDX, IDC_DRAWARCSINECHECK, m_bDrawArcsine);
	DDX_Check(pDX, IDC_DRAWARCCOSINECHECK, m_bDrawArccosine);
	DDX_Check(pDX, IDC_DRAWARCTANCHECK, m_bDrawArctan);
	DDX_Check(pDX, IDC_DRAWARCCOTANCHECK, m_bDrawArccotan);
	DDX_Check(pDX, IDC_DRAWHYPERBOLICSINECHECK, m_bDrawHyperbolicSine);
	DDX_Check(pDX, IDC_DRAWHYPERBOLICCOSINECHECK, m_bDrawHyperbolicCosine);
	DDX_Check(pDX, IDC_DRAWHYPERBOLICTANGENTCHECK, m_bDrawHyperbolicTan);
	DDX_Check(pDX, IDC_DRAWHYPERBOLICCOTANGENTCHECK, m_bDrawHyperbolicCotan);
}


BEGIN_MESSAGE_MAP(CDrawFunctionsDialog, CDialog)
	ON_BN_CLICKED(IDC_DRAWCUSTOMCHECK,             &CDrawFunctionsDialog::OnClickedDrawcustomcheck)
	ON_EN_CHANGE(IDC_FUNCEDIT,                     &CDrawFunctionsDialog::OnChangeExpressionEdit)
	ON_BN_CLICKED(IDC_DRAWSINECHECK,               &CDrawFunctionsDialog::OnClickedDrawsinecheck)
	ON_BN_CLICKED(IDC_DRAWCOSINECHECK, &CDrawFunctionsDialog::OnClickedDrawcosinecheck)
	ON_BN_CLICKED(IDC_DRAWTANCHECK, &CDrawFunctionsDialog::OnClickedDrawtancheck)
	ON_BN_CLICKED(IDC_DRAWCOTANCHECK, &CDrawFunctionsDialog::OnClickedDrawcotancheck)
	ON_BN_CLICKED(IDC_DRAWEXPCHECK, &CDrawFunctionsDialog::OnClickedDrawexpcheck)
	ON_BN_CLICKED(IDC_DRAWLNCHECK, &CDrawFunctionsDialog::OnClickedDrawlncheck)
	ON_BN_CLICKED(IDC_DRAWARCSINECHECK, &CDrawFunctionsDialog::OnClickedDrawarcsinecheck)
	ON_BN_CLICKED(IDC_DRAWARCCOSINECHECK, &CDrawFunctionsDialog::OnClickedDrawarccosinecheck)
	ON_BN_CLICKED(IDC_DRAWARCTANCHECK, &CDrawFunctionsDialog::OnClickedDrawarctancheck)
	ON_BN_CLICKED(IDC_DRAWARCCOTANCHECK, &CDrawFunctionsDialog::OnClickedDrawarccotancheck)
	ON_BN_CLICKED(IDC_DRAWHYPERBOLICSINECHECK, &CDrawFunctionsDialog::OnClickedDrawhyperbolicsinecheck)
	ON_BN_CLICKED(IDC_DRAWHYPERBOLICCOSINECHECK, &CDrawFunctionsDialog::OnClickedDrawhyperboliccosinecheck)
	ON_BN_CLICKED(IDC_DRAWHYPERBOLICTANGENTCHECK, &CDrawFunctionsDialog::OnClickedDrawhyperbolictangentcheck)
	ON_BN_CLICKED(IDC_DRAWHYPERBOLICCOTANGENTCHECK, &CDrawFunctionsDialog::OnClickedDrawhyperboliccotangentcheck)
END_MESSAGE_MAP()


// CDrawFunctionsDialog message handlers


void CDrawFunctionsDialog::OnClickedDrawsinecheck()
{
	// TODO: Add your control notification handler code here
	CButton* pSine = (CButton*) GetDlgItem(IDC_DRAWSINECHECK);

	if ( pSine->GetCheck() )
		m_bDrawSine = TRUE;

	if ( ! pSine->GetCheck() )
		m_bDrawSine = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *>(AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc =dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );
	ASSERT_VALID(pDoc);

	pDoc->m_bDrawSine = m_bDrawSine;

	UpdateData();
	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawcosinecheck()
{
	// TODO: Add your control notification handler code here
	CButton* pCosine = (CButton*) GetDlgItem(IDC_DRAWCOSINECHECK);

	if ( pCosine->GetCheck() )
		m_bDrawCosine = TRUE;

	if ( ! pCosine->GetCheck() )
		m_bDrawCosine = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *>(AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc =dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );
	ASSERT_VALID(pDoc);

	pDoc->m_bDrawCosine = m_bDrawCosine;

	UpdateData();
	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawtancheck()
{
	// TODO: Add your control notification handler code here
	CButton* pTan = (CButton*) GetDlgItem(IDC_DRAWTANCHECK);

	if ( pTan->GetCheck() )
		m_bDrawTan = TRUE;

	if ( ! pTan->GetCheck() )
		m_bDrawTan = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *>(AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );
	ASSERT_VALID(pDoc);

	pDoc->m_bDrawTan = m_bDrawTan;

	UpdateData();
	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawcotancheck()
{
	// TODO: Add your control notification handler code here
	CButton* pCotan = (CButton*) GetDlgItem(IDC_DRAWCOTANCHECK);

	if ( pCotan->GetCheck() )
		m_bDrawCotan = TRUE;

	if ( ! pCotan->GetCheck() )
		m_bDrawCotan = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *>(AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawCotan = m_bDrawCotan;

	UpdateData();
	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawexpcheck()
{
	// TODO: Add your control notification handler code here
	CButton* pExp = (CButton*) GetDlgItem(IDC_DRAWEXPCHECK );

	if ( pExp->GetCheck() )
		m_bDrawExp = TRUE;

	if ( ! pExp->GetCheck() )
		m_bDrawExp = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawExp = m_bDrawExp;

	UpdateData();
	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawlncheck()
{
	// TODO: Add your control notification handler code here
	CButton* pLN = (CButton*) GetDlgItem(IDC_DRAWLNCHECK);

	if ( pLN->GetCheck() )
		m_bDrawLN = TRUE;

	if ( ! pLN->GetCheck() )
		m_bDrawLN = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawLN = m_bDrawLN;

	UpdateData();
	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawarcsinecheck()
{
	// TODO: Add your control notification handler code here
	CButton* pArcsine = (CButton*) GetDlgItem(IDC_DRAWARCSINECHECK);

	if ( pArcsine->GetCheck() )
		m_bDrawArcsine = TRUE;

	if ( ! pArcsine->GetCheck() )
		m_bDrawArcsine = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawArcsine = m_bDrawArcsine;

	UpdateData();
	pDoc->UpdateAllViews(NULL);
	
}


void CDrawFunctionsDialog::OnClickedDrawarccosinecheck()
{
	// TODO: Add your control notification handler code here
	CButton* pArccosine = (CButton*) GetDlgItem(IDC_DRAWARCCOSINECHECK);

	if ( pArccosine->GetCheck() )
		m_bDrawArccosine = TRUE;

	if ( ! pArccosine->GetCheck() )
		m_bDrawArccosine = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> ( AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> ( pWnd->GetActiveDocument() );

	pDoc->m_bDrawArccosine = m_bDrawArccosine;

	UpdateData();
	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawarctancheck()
{
	// TODO: Add your control notification handler code here
	CButton* pArctan = (CButton*) GetDlgItem(IDC_DRAWARCTANCHECK);

	if ( pArctan->GetCheck() )
		m_bDrawArctan = TRUE;

	if ( ! pArctan->GetCheck() )
		m_bDrawArctan = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> ( pWnd->GetActiveDocument() );

	pDoc->m_bDrawArctan = m_bDrawArctan;

	UpdateData();

	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawarccotancheck()
{
	// TODO: Add your control notification handler code here
	CButton* pArccotan = (CButton*) GetDlgItem(IDC_DRAWARCCOTANCHECK);

	if ( pArccotan->GetCheck() )
		m_bDrawArccotan = TRUE;

	if ( ! pArccotan->GetCheck() )
		m_bDrawArccotan = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawArccotan = m_bDrawArccotan;

	UpdateData();

	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawhyperbolicsinecheck()
{
	// TODO: Add your control notification handler code here
	CButton* pHyperSine = (CButton*) GetDlgItem(IDC_DRAWHYPERBOLICSINECHECK);

	if ( pHyperSine->GetCheck() )
		m_bDrawHyperbolicSine = TRUE;

	if ( ! pHyperSine->GetCheck() )
		m_bDrawHyperbolicSine = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawHyperbolicSine = m_bDrawHyperbolicSine;

	UpdateData();

	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawhyperboliccosinecheck()
{
	// TODO: Add your control notification handler code here
	CButton* pHyperCos = (CButton*) GetDlgItem(IDC_DRAWHYPERBOLICCOSINECHECK);

	if ( pHyperCos->GetCheck() )
		m_bDrawHyperbolicCosine = TRUE;

	if ( ! pHyperCos->GetCheck() )
		m_bDrawHyperbolicCosine = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawHyperbolicCosine = m_bDrawHyperbolicCosine;

	UpdateData();

	pDoc->UpdateAllViews(NULL);


}


void CDrawFunctionsDialog::OnClickedDrawhyperbolictangentcheck()
{
	// TODO: Add your control notification handler code here
	CButton* pHyperTan = (CButton*) GetDlgItem(IDC_DRAWHYPERBOLICTANGENTCHECK);

	if ( pHyperTan->GetCheck() )
		m_bDrawHyperbolicTan = TRUE;

	if ( ! pHyperTan->GetCheck() )
		m_bDrawHyperbolicTan = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawHyperbolicTan = m_bDrawHyperbolicTan;

	UpdateData();

	pDoc->UpdateAllViews(NULL);
}


void CDrawFunctionsDialog::OnClickedDrawhyperboliccotangentcheck()
{
	// TODO: Add your control notification handler code here
	CButton* pHyperCot = (CButton*) GetDlgItem(IDC_DRAWHYPERBOLICCOTANGENTCHECK);

	if ( pHyperCot->GetCheck() )
		m_bDrawHyperbolicCotan = TRUE;

	if ( ! pHyperCot->GetCheck() )
		m_bDrawHyperbolicCotan = FALSE;

	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );

	CGraphDrawerDoc * pDoc = dynamic_cast<CGraphDrawerDoc *> (pWnd->GetActiveDocument() );

	pDoc->m_bDrawHyperbolicCotan = m_bDrawHyperbolicCotan;

	UpdateData();

	pDoc->UpdateAllViews(NULL);
	
}


// ---------------------------------------------------------------------------
// Custom expression handlers
// ---------------------------------------------------------------------------

void CDrawFunctionsDialog::OnClickedDrawcustomcheck()
{
	CButton* pCheck = (CButton*)GetDlgItem(IDC_DRAWCUSTOMCHECK);
	m_bDrawCustomFunction = pCheck->GetCheck() ? TRUE : FALSE;

	CMainFrame* pWnd = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	CGraphDrawerDoc* pDoc = dynamic_cast<CGraphDrawerDoc*>(pWnd->GetActiveDocument());
	if (!pDoc) return;

	// UpdateData returns FALSE (and shows a warning) if a field is non-numeric.
	if (!UpdateData(TRUE)) return;

	if (m_dRangeStart >= m_dRangeEnd)
	{
		AfxMessageBox(_T("Range start must be less than range end."), MB_OK | MB_ICONWARNING);
		return;
	}

	pDoc->SetCustomExpression(m_strExpression, m_bDrawCustomFunction,
	                          m_dRangeStart, m_dRangeEnd);
}


void CDrawFunctionsDialog::OnChangeExpressionEdit()
{
	// Only trigger a redraw if custom drawing is already enabled
	CButton* pCheck = (CButton*)GetDlgItem(IDC_DRAWCUSTOMCHECK);
	if (!pCheck || !pCheck->GetCheck())
		return;

	CMainFrame* pWnd = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	CGraphDrawerDoc* pDoc = dynamic_cast<CGraphDrawerDoc*>(pWnd->GetActiveDocument());
	if (!pDoc) return;

	if (!UpdateData(TRUE)) return;

	if (m_dRangeStart >= m_dRangeEnd)
		return;  // silent: user is still typing — no popup on every keystroke

	pDoc->SetCustomExpression(m_strExpression, TRUE,
	                          m_dRangeStart, m_dRangeEnd);
}


// ---------------------------------------------------------------------------
// Modeless dialog lifecycle
// ---------------------------------------------------------------------------

// When the dialog is shown (first time or after ShowWindow), sync checkboxes
// from the document so the UI reflects the current drawing state.
BOOL CDrawFunctionsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame* pWnd = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (!pWnd) return TRUE;
	CGraphDrawerDoc* pDoc = dynamic_cast<CGraphDrawerDoc*>(pWnd->GetActiveDocument());
	if (!pDoc) return TRUE;

	m_bDrawSine             = pDoc->m_bDrawSine;
	m_bDrawCosine           = pDoc->m_bDrawCosine;
	m_bDrawTan              = pDoc->m_bDrawTan;
	m_bDrawCotan            = pDoc->m_bDrawCotan;
	m_bDrawExp              = pDoc->m_bDrawExp;
	m_bDrawLN               = pDoc->m_bDrawLN;
	m_bDrawArcsine          = pDoc->m_bDrawArcsine;
	m_bDrawArccosine        = pDoc->m_bDrawArccosine;
	m_bDrawArctan           = pDoc->m_bDrawArctan;
	m_bDrawArccotan         = pDoc->m_bDrawArccotan;
	m_bDrawHyperbolicSine   = pDoc->m_bDrawHyperbolicSine;
	m_bDrawHyperbolicCosine = pDoc->m_bDrawHyperbolicCosine;
	m_bDrawHyperbolicTan    = pDoc->m_bDrawHyperbolicTan;
	m_bDrawHyperbolicCotan  = pDoc->m_bDrawHyperbolicCotan;
	m_bDrawCustomFunction   = pDoc->m_bDrawCustomFunction;
	m_strExpression         = pDoc->m_strCustomExpression;
	m_dRangeStart           = pDoc->m_dCustomRangeStart;
	m_dRangeEnd             = pDoc->m_dCustomRangeEnd;

	UpdateData(FALSE);
	return TRUE;
}

// For a modeless dialog OK means "apply all settings and keep the dialog open".
void CDrawFunctionsDialog::OnOK()
{
	// Read all controls; returns FALSE (with a built-in warning) if a field is invalid.
	if (!UpdateData(TRUE)) return;

	CMainFrame* pWnd = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (!pWnd) return;
	CGraphDrawerDoc* pDoc = dynamic_cast<CGraphDrawerDoc*>(pWnd->GetActiveDocument());
	if (!pDoc) return;

	pDoc->m_bDrawSine             = m_bDrawSine;
	pDoc->m_bDrawCosine           = m_bDrawCosine;
	pDoc->m_bDrawTan              = m_bDrawTan;
	pDoc->m_bDrawCotan            = m_bDrawCotan;
	pDoc->m_bDrawExp              = m_bDrawExp;
	pDoc->m_bDrawLN               = m_bDrawLN;
	pDoc->m_bDrawArcsine          = m_bDrawArcsine;
	pDoc->m_bDrawArccosine        = m_bDrawArccosine;
	pDoc->m_bDrawArctan           = m_bDrawArctan;
	pDoc->m_bDrawArccotan         = m_bDrawArccotan;
	pDoc->m_bDrawHyperbolicSine   = m_bDrawHyperbolicSine;
	pDoc->m_bDrawHyperbolicCosine = m_bDrawHyperbolicCosine;
	pDoc->m_bDrawHyperbolicTan    = m_bDrawHyperbolicTan;
	pDoc->m_bDrawHyperbolicCotan  = m_bDrawHyperbolicCotan;

	// Apply custom expression (also starts background thread if enabled)
	if (m_dRangeStart >= m_dRangeEnd)
	{
		AfxMessageBox(_T("Range start must be less than range end."), MB_OK | MB_ICONWARNING);
		return;
	}
	pDoc->SetCustomExpression(m_strExpression, m_bDrawCustomFunction,
	                          m_dRangeStart, m_dRangeEnd);

	pDoc->UpdateAllViews(NULL);
	// Do NOT call CDialog::OnOK() — that would destroy the modeless window.
}

// Cancel just hides the dialog; settings already applied by individual handlers.
void CDrawFunctionsDialog::OnCancel()
{
	ShowWindow(SW_HIDE);
	CMainFrame* pWnd = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (pWnd) pWnd->SetDrawFuncVisible(FALSE);
}
