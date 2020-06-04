// DrawOptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDrawer.h"
#include "DrawOptionsDialog.h"
#include "afxdialogex.h"


// CDrawOptionsDialog dialog

IMPLEMENT_DYNAMIC(CDrawOptionsDialog, CDialog)

CDrawOptionsDialog::CDrawOptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawOptionsDialog::IDD, pParent)
	, m_bShowCoordinateAxes(FALSE)	
	, m_bShowTicks(FALSE)
	, m_bShowTicksLabel(FALSE)
	, m_nTicksInterval(0)
	, m_clrAxesColor(RGB(0,0,255))
	, m_nAxesPenWidth(1)
	, m_clrBkgndColor(RGB(96, 96, 96) )
{

	
}

CDrawOptionsDialog::~CDrawOptionsDialog()
{
}

void CDrawOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SHOWCOORDINATEAXESCHECK, m_bShowCoordinateAxes);
	DDX_Check(pDX, IDC_SHOWTICKSCHECK, m_bShowTicks);
	DDX_Check(pDX, IDC_SHOWTICKSLABELCHECK, m_bShowTicksLabel);
	DDX_Text(pDX, IDC_TICKSINTERVALEDIT, m_nTicksInterval);
	DDX_Control(pDX, IDC_TICKSINTERVALEDIT, m_editTicksInterval);
	DDX_Control(pDX, IDC_MFCCOLORBUTTONCOORD, m_wndCoordAxesColor);
	DDX_Control(pDX, IDC_PENWIDTHCOMBO, m_cbPenWidth);
	DDX_Control(pDX, IDC_BKGNDMFCCOLORBUTTON, m_wndBkgndColor);
}


BEGIN_MESSAGE_MAP(CDrawOptionsDialog, CDialog)
	ON_BN_CLICKED(IDC_SHOWCOORDINATEAXESCHECK, &CDrawOptionsDialog::OnClickedShowcoordinateaxescheck)
	ON_BN_CLICKED(IDC_SHOWTICKSCHECK, &CDrawOptionsDialog::OnClickedShowtickscheck)
	ON_BN_CLICKED(IDC_SHOWTICKSLABELCHECK, &CDrawOptionsDialog::OnClickedShowtickslabelcheck)
	ON_BN_CLICKED(IDOK, &CDrawOptionsDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTONCOORD, &CDrawOptionsDialog::OnClickedMfccolorbuttoncoord)
	ON_CBN_SELCHANGE(IDC_PENWIDTHCOMBO, &CDrawOptionsDialog::OnSelchangePenwidthcombo)
	ON_BN_CLICKED(IDC_BKGNDMFCCOLORBUTTON, &CDrawOptionsDialog::OnClickedBkgndmfccolorbutton)
END_MESSAGE_MAP()


// CDrawOptionsDialog message handlers


void CDrawOptionsDialog::OnClickedShowcoordinateaxescheck()
{
	// TODO: Add your control notification handler code here
	// Get a pointer to each of the check box object
	CButton* pCoordAxes = (CButton*) GetDlgItem(IDC_SHOWCOORDINATEAXESCHECK);
	CButton* pShowTicks = (CButton*) GetDlgItem ( IDC_SHOWTICKSCHECK );
	CButton* pShowTicksLabel = (CButton*) GetDlgItem (IDC_SHOWTICKSLABELCHECK);

	if ( pCoordAxes->GetCheck() )
		m_bShowCoordinateAxes = TRUE;

	if ( ! pCoordAxes->GetCheck() )
	{
		m_bShowCoordinateAxes = FALSE;
		pShowTicks->SetCheck(0);
		m_bShowTicks = FALSE;
		pShowTicksLabel->SetCheck(0);
		m_bShowTicksLabel = FALSE;
	}
}


void CDrawOptionsDialog::OnClickedShowtickscheck()
{
	// TODO: Add your control notification handler code here
	CButton* pShowTicks = (CButton*) GetDlgItem(IDC_SHOWTICKSCHECK);
	CButton* pCoordAxes = (CButton*) GetDlgItem(IDC_SHOWCOORDINATEAXESCHECK);
	CButton* pShowTicksLabel = (CButton*) GetDlgItem (IDC_SHOWTICKSLABELCHECK);

	if ( pShowTicks->GetCheck() ) {
		m_bShowTicks = TRUE;
		pCoordAxes->SetCheck(1);
		m_bShowCoordinateAxes = TRUE;
	}

	if ( ! pShowTicks->GetCheck() )
	{
		m_bShowTicks = FALSE;
		pShowTicksLabel->SetCheck(0);
		m_bShowTicksLabel = FALSE;
	}

}


void CDrawOptionsDialog::OnClickedShowtickslabelcheck()
{
	// TODO: Add your control notification handler code here
	CButton* pShowTicksLabel = (CButton*) GetDlgItem(IDC_SHOWTICKSLABELCHECK);
	CButton* pShowTicks = (CButton*) GetDlgItem ( IDC_SHOWTICKSCHECK);
	CButton* pCoordAxes = (CButton*) GetDlgItem (IDC_SHOWCOORDINATEAXESCHECK);

	if ( pShowTicksLabel->GetCheck() ) {
		m_bShowTicksLabel = TRUE;
		pCoordAxes->SetCheck(1);
		m_bShowCoordinateAxes = TRUE;
		pShowTicks->SetCheck(1);
		m_bShowTicks = TRUE;
	}

	if ( ! pShowTicksLabel->GetCheck() ) 
		m_bShowTicksLabel = FALSE;
}


void CDrawOptionsDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	// Transfer data from the controls to the variables
	UpdateData();

	CDialog::OnOK();
}



BOOL CDrawOptionsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	// m_editTicksInterval.SetWindowText(_T("100"));

	PopulatePenWidthCombo();
    // m_cbPenWidth.SetCurSel(0);
	// CString strGetSel;
	// strGetSel.Format(_T("Current selection is %d"), m_cbPenWidth.GetCurSel() );
	// AfxMessageBox(strGetSel);
	// m_wndCoordAxesColor.SetColor(RGB(0,0,255));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// Populate Pen Width Combo box
void CDrawOptionsDialog::PopulatePenWidthCombo(void)
{
	
	int i =0;
	
	for (i=0; i< 10; i++){
		CString strTmpOne;
		strTmpOne.Format(_T("%d pt"), i+1);
		m_cbPenWidth.AddString(strTmpOne);
	}
	

	// m_cbPenWidth.AddString(_T("1 pt"));

	/*
	for ( int i = 0; i< 10; i++)
	{
	COMBOBOXEXITEM cbi = {0};
	CString str;
	str.Format(_T("%d pt"), i +1);
	int nItem;

	cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY  | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

	cbi.iItem = i;
	cbi.pszText = (LPTSTR) (LPCTSTR) str;
	cbi.iImage = 0;
	cbi.iSelectedImage = 1;
	cbi.iOverlay = 2;
	cbi.iIndent = ( 0 & 0x03 ); // Set indentation according to item position

	nItem = m_cbPenWidth.InsertItem (&cbi);
	ASSERT(nItem == i );
	}
	*/
	
}



void CDrawOptionsDialog::OnClickedMfccolorbuttoncoord()
{
	// TODO: Add your control notification handler code here
	m_clrAxesColor = m_wndCoordAxesColor.GetColor();	
	UpdateData();
}

void CDrawOptionsDialog::GetData(DrawOptionsData* dr)
{
	
	// Check Boxes
	dr->m_bCheckStateShowCoordinateAxes = m_bShowCoordinateAxes;
	dr->m_bCheckStateShowTicks = m_bShowTicks;
	dr->m_bCheckStateShowTicksLabel = m_bShowTicksLabel;

	// Edit boxes
	dr->m_nTicksInterval = m_nTicksInterval;

	/*
	// Combo Boxes
	// PopulatePenWidthCombo();
	// yourComboBox.SetCurSel(yourComboBox.FindStringExact(0, yourString));
	// dr->strCoordAxesThickness = 
	int nIndex = m_cbPenWidth.GetCurSel();
	// CString strTmp;
	// strTmp.Format(_T("Combobox index is: %d"), nIndex);
	// if ( nIndex < 0 ) 
	//	AfxMessageBox(strTmp);
	//int nCount = m_cbPenWidth.GetCount();

	if ( nIndex != CB_ERR)
	{
		// dr->nIndexThickness = nIndex;
		m_cbPenWidth.GetLBText( nIndex, dr->strCoordAxesThickness);
	}
	*/

	// AfxMessageBox(dr->strCoordAxesThickness);
	
	dr->clrCoordColor = m_clrAxesColor;
	// Background color
	dr->clrBkgndColor = m_clrBkgndColor;
}

void CDrawOptionsDialog::SetData(DrawOptionsData dr)
{
	// Check boxes
	m_bShowCoordinateAxes = dr.m_bCheckStateShowCoordinateAxes;
	m_bShowTicks = dr.m_bCheckStateShowTicks;
	m_bShowTicksLabel = dr.m_bCheckStateShowTicksLabel;

	// Edit boxes
	m_nTicksInterval = dr.m_nTicksInterval;
	
	/*
	// Combo boxes
	// PopulatePenWidthCombo();
	//CString strTmp(dr.strCoordAxesThickness);
	// int nIndex = dr.nIndexThickness;
   m_cbPenWidth.SetCurSel(m_cbPenWidth.FindStringExact(0, dr.strCoordAxesThickness));
	// int nCount = m_cbPenWidth.GetCount();
	// if ( nIndex !=CB_ERR )
	//  m_cbPenWidth.SetCurSel(nIndex);
	*/

	m_wndCoordAxesColor.SetColor(dr.clrCoordColor);
	// Background color
	m_wndBkgndColor.SetColor(dr.clrBkgndColor);
}


void CDrawOptionsDialog::OnSelchangePenwidthcombo()
{
	// TODO: Add your control notification handler code here
		// ** Retrieve the index of the selected item
	int nIndex = m_cbPenWidth.GetCurSel();
	
	// ** Check if the index is valid

	if ( nIndex != CB_ERR )
	{
		m_nAxesPenWidth = nIndex + 1;
		// PopulatePenWidthCombo();
		 UpdateData();
	}
}


void CDrawOptionsDialog::OnClickedBkgndmfccolorbutton()
{
	// TODO: Add your control notification handler code here
	m_clrBkgndColor = m_wndBkgndColor.GetColor();
	UpdateData();
}
