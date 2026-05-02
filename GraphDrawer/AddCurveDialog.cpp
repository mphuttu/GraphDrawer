#include "stdafx.h"
#include "GraphDrawer.h"     // pulls in resource.h
#include "AddCurveDialog.h"
#include "resource.h"
#include "ExpressionParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ---------------------------------------------------------------------------
// Helper: evaluate a range-field string as a constant expression.
// Accepts plain numbers ("3.14") or expressions ("2*pi", "-10", "e^2").
// ---------------------------------------------------------------------------
static bool ParseRangeExpr(const CString& str, double& result)
{
	CExpressionParser p;
	if (p.Parse(str))
	{
		double v = 0.0;
		if (p.Evaluate(0.0, v) && _finite(v))
		{
			result = v;
			return true;
		}
	}
	return false;
}


IMPLEMENT_DYNAMIC(CAddCurveDialog, CDialog)

CAddCurveDialog::CAddCurveDialog(CWnd* pParent)
	: CDialog(CAddCurveDialog::IDD, pParent)
	, m_nCurveType(0)
	, m_strLabel(L"")
	, m_strExprYFX(L"")
	, m_strXFrom(L"-10")
	, m_strXTo(L"10")
	, m_strExprParX(L"")
	, m_strExprParY(L"")
	, m_strTFrom(L"0")
	, m_strTTo(L"2*pi")
	, m_strPhiFrom(L"0")
	, m_strPhiTo(L"2*pi")
	, m_strXFromImp(L"-2")
	, m_strXToImp(L"2")
	, m_strYFromImp(L"-2")
	, m_strYToImp(L"2")
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
	DDX_Text   (pDX, IDC_CURVE_RANGE_FROM,        m_strXFrom);
	DDX_Text   (pDX, IDC_CURVE_RANGE_TO,          m_strXTo);
	// Parametric
	DDX_Text   (pDX, IDC_CURVE_PARAM_X_EDIT,      m_strExprParX);
	DDX_Text   (pDX, IDC_CURVE_PARAM_Y_EDIT,      m_strExprParY);
	DDX_Text   (pDX, IDC_CURVE_TRANGE_FROM,       m_strTFrom);
	DDX_Text   (pDX, IDC_CURVE_TRANGE_TO,         m_strTTo);
	// Polar
	DDX_Text   (pDX, IDC_CURVE_EXPR_POLAR_EDIT,   m_strExprPolar);
	DDX_Text   (pDX, IDC_POLAR_PHI_FROM,          m_strPhiFrom);
	DDX_Text   (pDX, IDC_POLAR_PHI_TO,            m_strPhiTo);
	// Implicit
	DDX_Text   (pDX, IDC_CURVE_EXPR_IMPLICIT_EDIT, m_strExprImplicit);
	DDX_Text   (pDX, IDC_IMPLICIT_X_FROM, m_strXFromImp);
	DDX_Text   (pDX, IDC_IMPLICIT_X_TO, m_strXToImp);
	DDX_Text   (pDX, IDC_IMPLICIT_Y_FROM, m_strYFromImp);
	DDX_Text   (pDX, IDC_IMPLICIT_Y_TO, m_strYToImp);

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
	if (!m_strCaption.IsEmpty())
		SetWindowText(m_strCaption);
	if (m_nCurveType == 2) {
		if (m_strExprPolar.IsEmpty()) m_strExprPolar = L"1 - cos(k)";
	}
	if (m_nCurveType == 3) {
		if (m_strExprImplicit.IsEmpty()) m_strExprImplicit = L"pow(abs(x),2.0/3)+pow(abs(y),2.0/3)-1";
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

	// Validate inputs and parse range expressions (pi, e, arithmetic allowed).
	if (m_nCurveType == 0)
	{
		if (m_strExprYFX.IsEmpty())
		{
			AfxMessageBox(L"Please enter a y = f(x) expression.", MB_ICONWARNING);
			return;
		}
		double dXFrom = 0.0, dXTo = 0.0;
		if (!ParseRangeExpr(m_strXFrom, dXFrom))
		{
			AfxMessageBox(L"Invalid x start value. Use a number or an expression like '-pi'.", MB_ICONWARNING);
			return;
		}
		if (!ParseRangeExpr(m_strXTo, dXTo))
		{
			AfxMessageBox(L"Invalid x end value. Use a number or an expression like 'pi'.", MB_ICONWARNING);
			return;
		}
		if (dXFrom >= dXTo)
		{
			AfxMessageBox(L"x start must be less than x end.", MB_ICONWARNING);
			return;
		}
		m_result.type     = UCT_YFX;
		m_result.strExprY = m_strExprYFX;
		m_result.xStart   = dXFrom;
		m_result.xEnd     = dXTo;
	}
	else if (m_nCurveType == 1)
	{
		if (m_strExprParX.IsEmpty() || m_strExprParY.IsEmpty())
		{
			AfxMessageBox(L"Please enter both x(t) and y(t) expressions.", MB_ICONWARNING);
			return;
		}
		double dTFrom = 0.0, dTTo = 0.0;
		if (!ParseRangeExpr(m_strTFrom, dTFrom))
		{
			AfxMessageBox(L"Invalid t start value.", MB_ICONWARNING);
			return;
		}
		if (!ParseRangeExpr(m_strTTo, dTTo))
		{
			AfxMessageBox(L"Invalid t end value. Use a number or an expression like '2*pi'.", MB_ICONWARNING);
			return;
		}
		if (dTFrom >= dTTo)
		{
			AfxMessageBox(L"t start must be less than t end.", MB_ICONWARNING);
			return;
		}
		m_result.type        = UCT_PARAMETRIC;
		m_result.strExprX    = m_strExprParX;
		m_result.strExprYPar = m_strExprParY;
		m_result.tStart      = dTFrom;
		m_result.tEnd        = dTTo;
	}
	else if (m_nCurveType == 2)
	{
		if (m_strExprPolar.IsEmpty())
		{
			AfxMessageBox(L"Please enter a polar expression r = f(k).", MB_ICONWARNING);
			return;
		}
		double dPhiFrom = 0.0, dPhiTo = 0.0;
		if (!ParseRangeExpr(m_strPhiFrom, dPhiFrom))
		{
			AfxMessageBox(L"Invalid k start value.", MB_ICONWARNING);
			return;
		}
		if (!ParseRangeExpr(m_strPhiTo, dPhiTo))
		{
			AfxMessageBox(L"Invalid k end value. Use a number or an expression like '2*pi'.", MB_ICONWARNING);
			return;
		}
		if (dPhiFrom >= dPhiTo)
		{
			AfxMessageBox(L"k start must be less than k end.", MB_ICONWARNING);
			return;
		}
		m_result.type         = UCT_POLAR;
		m_result.strExprPolar = m_strExprPolar;
		m_result.phiStart     = dPhiFrom;
		m_result.phiEnd       = dPhiTo;
	}
	else if (m_nCurveType == 3)
	{
		if (m_strExprImplicit.IsEmpty())
		{
			AfxMessageBox(L"Please enter an implicit expression f(x, y) = 0.", MB_ICONWARNING);
			return;
		}
		double dXFromImp = 0.0, dXToImp = 0.0, dYFromImp = 0.0, dYToImp = 0.0;
		if (!ParseRangeExpr(m_strXFromImp, dXFromImp) || !ParseRangeExpr(m_strXToImp, dXToImp) ||
		    !ParseRangeExpr(m_strYFromImp, dYFromImp) || !ParseRangeExpr(m_strYToImp, dYToImp))
		{
			AfxMessageBox(L"Invalid implicit range value.", MB_ICONWARNING);
			return;
		}
		if (dXFromImp >= dXToImp || dYFromImp >= dYToImp)
		{
			AfxMessageBox(L"x/y start must be less than x/y end.", MB_ICONWARNING);
			return;
		}
		m_result.type            = UCT_IMPLICIT;
		m_result.strExprImplicit = m_strExprImplicit;
		m_result.xStartImp       = dXFromImp;
		m_result.xEndImp         = dXToImp;
		m_result.yStartImp       = dYFromImp;
		m_result.yEndImp         = dYToImp;
	}

	// Common fields.
	m_result.bVisible = TRUE;
	m_result.color    = m_color;
	m_result.label    = m_strLabel.IsEmpty() ? L"Curve" : m_strLabel;

	CDialog::OnOK();
}

// ---------------------------------------------------------------------------
// PrePopulate — fill dialog fields from an existing UserCurve before DoModal.
// ---------------------------------------------------------------------------
void CAddCurveDialog::PrePopulate(const UserCurve& curve)
{
	m_nCurveType = (int)curve.type;
	m_strLabel   = curve.label;
	m_color      = curve.color;
	m_strCaption = L"Edit Curve";

	// y = f(x)
	m_strExprYFX = curve.strExprY;
	m_strXFrom.Format(L"%g", curve.xStart);
	m_strXTo.Format(L"%g", curve.xEnd);

	// Parametric
	m_strExprParX = curve.strExprX;
	m_strExprParY = curve.strExprYPar;
	m_strTFrom.Format(L"%g", curve.tStart);
	m_strTTo.Format(L"%g", curve.tEnd);

	// Polar
	m_strExprPolar = curve.strExprPolar;
	m_strPhiFrom.Format(L"%g", curve.phiStart);
	m_strPhiTo.Format(L"%g", curve.phiEnd);

	// Implicit
	m_strExprImplicit = curve.strExprImplicit;
	m_strXFromImp.Format(L"%g", curve.xStartImp);
	m_strXToImp.Format(L"%g", curve.xEndImp);
	m_strYFromImp.Format(L"%g", curve.yStartImp);
	m_strYToImp.Format(L"%g", curve.yEndImp);
}
