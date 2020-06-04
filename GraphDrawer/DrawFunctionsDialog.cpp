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
	DDX_Check(pDX, IDC_DRAWSINECHECK, m_bDrawSine);
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
	ON_BN_CLICKED(IDC_DRAWSINECHECK, &CDrawFunctionsDialog::OnClickedDrawsinecheck)
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
