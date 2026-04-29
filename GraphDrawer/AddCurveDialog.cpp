#include "stdafx.h"
#include "GraphDrawer.h"     // pulls in resource.h
#include "AddCurveDialog.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CAddCurveDialog, CDialog)

CAddCurveDialog::CAddCurveDialog(CWnd* pParent)
	: CDialog(CAddCurveDialog::IDD, pParent)
	, m_nCurveType(0)
	, m_strLabel(L"")
	, m_strExprYFX(L"")
	, m_dXFrom(-10.0)
	, m_dXTo(10.0)
	, m_strExprParX(L"")
	, m_strExprParY(L"")
	, m_dTFrom(0.0)
	, m_dTTo(6.2831853071795864769)   // 2*PI
	, m_color(RGB(255, 255, 0))
{
}

CAddCurveDialog::~CAddCurveDialog()
{
}

void CAddCurveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio  (pDX, IDC_CURVETYPE_YFX,         m_nCurveType);
	DDX_Text   (pDX, IDC_CURVE_LABEL_EDIT,       m_strLabel);
	// y = f(x)
	DDX_Text   (pDX, IDC_CURVE_EXPR_YFX_EDIT,    m_strExprYFX);
	DDX_Text   (pDX, IDC_CURVE_RANGE_FROM,        m_dXFrom);
	DDX_Text   (pDX, IDC_CURVE_RANGE_TO,          m_dXTo);
	// Parametric
	DDX_Text   (pDX, IDC_CURVE_PARAM_X_EDIT,      m_strExprParX);
	DDX_Text   (pDX, IDC_CURVE_PARAM_Y_EDIT,      m_strExprParY);
	DDX_Text   (pDX, IDC_CURVE_TRANGE_FROM,       m_dTFrom);
	DDX_Text   (pDX, IDC_CURVE_TRANGE_TO,         m_dTTo);
	// Polar
	DDX_Text   (pDX, IDC_CURVE_EXPR_POLAR_EDIT,   m_strExprPolar);
	DDX_Text   (pDX, IDC_POLAR_PHI_FROM,          m_dPhiFrom);
	DDX_Text   (pDX, IDC_POLAR_PHI_TO,            m_dPhiTo);
	// Implicit
	DDX_Text   (pDX, IDC_CURVE_EXPR_IMPLICIT_EDIT, m_strExprImplicit);
	DDX_Text   (pDX, IDC_IMPLICIT_X_FROM, m_dXFromImp);
	DDX_Text   (pDX, IDC_IMPLICIT_X_TO, m_dXToImp);
	DDX_Text   (pDX, IDC_IMPLICIT_Y_FROM, m_dYFromImp);
	DDX_Text   (pDX, IDC_IMPLICIT_Y_TO, m_dYToImp);

	DDX_Control(pDX, IDC_CURVE_COLOR_BTN,         m_wndColor);
}

BEGIN_MESSAGE_MAP(CAddCurveDialog, CDialog)
	ON_BN_CLICKED(IDC_CURVETYPE_YFX,   &CAddCurveDialog::OnClickedYFX)
	ON_BN_CLICKED(IDC_CURVETYPE_PARAM, &CAddCurveDialog::OnClickedParam)
	ON_BN_CLICKED(IDC_CURVETYPE_POLAR, &CAddCurveDialog::OnClickedPolar)
	ON_BN_CLICKED(IDC_CURVETYPE_IMPLICIT, &CAddCurveDialog::OnClickedImplicit)
END_MESSAGE_MAP()


BOOL CAddCurveDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set initial color on the color button.
	m_wndColor.SetColor(m_color);
	if (m_nCurveType == 2) {
		if (m_strExprPolar.IsEmpty()) m_strExprPolar = L"1 - cos(k)";
		if (m_dPhiFrom == 0.0 && m_dPhiTo == 0.0) { m_dPhiFrom = 0.0; m_dPhiTo = 6.283185307179586; }
	}
	if (m_nCurveType == 3) {
		if (m_strExprImplicit.IsEmpty()) m_strExprImplicit = L"pow(abs(x),2.0/3)+pow(abs(y),2.0/3)-1";
		if (m_dXFromImp == -2.0 && m_dXToImp == 2.0 && m_dYFromImp == -2.0 && m_dYToImp == 2.0) {
			m_dXFromImp = -2.0; m_dXToImp = 2.0; m_dYFromImp = -2.0; m_dYToImp = 2.0;
		}
	}
	UpdateData(FALSE);
	UpdateControlVisibility();
	return TRUE;
}

void CAddCurveDialog::UpdateControlVisibility()
{
	BOOL bYFX   = (m_nCurveType == 0);
	BOOL bParam = (m_nCurveType == 1);
	BOOL bPolar = (m_nCurveType == 2);
	BOOL bImpl  = (m_nCurveType == 3);

	// y = f(x) group — show/hide groupbox, labels and edits
	GetDlgItem(IDC_YFX_GROUP)           ->ShowWindow(bYFX ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_YFX_Y)         ->ShowWindow(bYFX ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_YFX_XRANGE)    ->ShowWindow(bYFX ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_YFX_COMMA)     ->ShowWindow(bYFX ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_YFX_RBRACE)    ->ShowWindow(bYFX ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_EXPR_YFX_EDIT) ->ShowWindow(bYFX ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_RANGE_FROM)    ->ShowWindow(bYFX ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_RANGE_TO)      ->ShowWindow(bYFX ? SW_SHOW : SW_HIDE);

	// Parametric group — show/hide groupbox, labels and edits
	GetDlgItem(IDC_PARAM_GROUP)         ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_PAR_XT)        ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_PAR_YT)        ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_PAR_TRANGE)    ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_PAR_COMMA)     ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_PAR_RBRACE)    ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_PARAM_X_EDIT)  ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_PARAM_Y_EDIT)  ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_TRANGE_FROM)   ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_TRANGE_TO)     ->ShowWindow(bParam ? SW_SHOW : SW_HIDE);

	// Polar group — show/hide groupbox, labels and edits
	GetDlgItem(IDC_POLAR_GROUP)         ->ShowWindow(bPolar ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_POLAR_R)       ->ShowWindow(bPolar ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_EXPR_POLAR_EDIT)->ShowWindow(bPolar ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_POLAR_PHIRANGE)->ShowWindow(bPolar ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_POLAR_PHI_FROM)      ->ShowWindow(bPolar ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_POLAR_COMMA)   ->ShowWindow(bPolar ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_POLAR_PHI_TO)        ->ShowWindow(bPolar ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_POLAR_RBRACE)  ->ShowWindow(bPolar ? SW_SHOW : SW_HIDE);

	// Implicit group
	GetDlgItem(IDC_IMPLICIT_GROUP)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_IMPLICIT_FXY)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_CURVE_EXPR_IMPLICIT_EDIT)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_IMPLICIT_XRANGE)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_IMPLICIT_X_FROM)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_IMPLICIT_XCOMMA)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_IMPLICIT_X_TO)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_IMPLICIT_YRANGE)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_IMPLICIT_Y_FROM)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_IMPLICIT_YCOMMA)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_IMPLICIT_Y_TO)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LABEL_IMPLICIT_RBRACE)->ShowWindow(bImpl ? SW_SHOW : SW_HIDE);
}

void CAddCurveDialog::OnClickedYFX()
{
	m_nCurveType = 0;
	UpdateControlVisibility();
}

void CAddCurveDialog::OnClickedParam()
{
	m_nCurveType = 1;
	UpdateControlVisibility();
}

// Add this handler:
void CAddCurveDialog::OnClickedPolar()
{
	m_nCurveType = 2;
	UpdateControlVisibility();
}

void CAddCurveDialog::OnClickedImplicit()
{
	m_nCurveType = 3;
	UpdateControlVisibility();
}

void CAddCurveDialog::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	// Retrieve chosen color.
	m_color = m_wndColor.GetColor();
	if (m_color == CLR_DEFAULT || m_color == CLR_NONE)
		m_color = RGB(255, 255, 0);

	// Validate inputs.
	if (m_nCurveType == 0)
	{
		if (m_strExprYFX.IsEmpty())
		{
			AfxMessageBox(L"Please enter a y = f(x) expression.", MB_ICONWARNING);
			return;
		}
		if (m_dXFrom >= m_dXTo)
		{
			AfxMessageBox(L"x start must be less than x end.", MB_ICONWARNING);
			return;
		}
	}
	else if (m_nCurveType == 1)
	{
		if (m_strExprParX.IsEmpty() || m_strExprParY.IsEmpty())
		{
			AfxMessageBox(L"Please enter both x(t) and y(t) expressions.", MB_ICONWARNING);
			return;
		}
		if (m_dTFrom >= m_dTTo)
		{
			AfxMessageBox(L"t start must be less than t end.", MB_ICONWARNING);
			return;
		}
	}
	else if (m_nCurveType == 2)
	{
		if (m_strExprPolar.IsEmpty())
		{
			AfxMessageBox(L"Please enter a polar expression r = f(k).", MB_ICONWARNING);
			return;
		}
		if (m_dPhiFrom >= m_dPhiTo)
		{
			AfxMessageBox(L"k start must be less than k end.", MB_ICONWARNING);
			return;
		}
	}
	else if (m_nCurveType == 3)
	{
		if (m_strExprImplicit.IsEmpty())
		{
			AfxMessageBox(L"Please enter an implicit expression f(x, y) = 0.", MB_ICONWARNING);
			return;
		}
		if (m_dXFromImp >= m_dXToImp || m_dYFromImp >= m_dYToImp)
		{
			AfxMessageBox(L"x/y start must be less than x/y end.", MB_ICONWARNING);
			return;
		}
	}

	// Build result curve.
	m_result.bVisible = TRUE;
	m_result.color    = m_color;
	m_result.label    = m_strLabel.IsEmpty() ? L"Curve" : m_strLabel;

	if (m_nCurveType == 0)
	{
		m_result.type     = UCT_YFX;
		m_result.strExprY = m_strExprYFX;
		m_result.xStart   = m_dXFrom;
		m_result.xEnd     = m_dXTo;
	}
	else if (m_nCurveType == 1)
	{
		m_result.type       = UCT_PARAMETRIC;
		m_result.strExprX   = m_strExprParX;
		m_result.strExprYPar= m_strExprParY;
		m_result.tStart     = m_dTFrom;
		m_result.tEnd       = m_dTTo;
	}
	else if (m_nCurveType == 2)
	{
		m_result.type         = UCT_POLAR;
		m_result.strExprPolar = m_strExprPolar;
		m_result.phiStart     = m_dPhiFrom;
		m_result.phiEnd       = m_dPhiTo;
	}
	else if (m_nCurveType == 3)
	{
		m_result.type = UCT_IMPLICIT;
		m_result.strExprImplicit = m_strExprImplicit;
		m_result.xStartImp = m_dXFromImp;
		m_result.xEndImp = m_dXToImp;
		m_result.yStartImp = m_dYFromImp;
		m_result.yEndImp = m_dYToImp;
	}

	CDialog::OnOK();
}
