// TriangleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDrawer.h"
#include "TriangleDialog.h"
#include "afxdialogex.h"
#include <cmath>

// CTriangleDialog dialog

IMPLEMENT_DYNAMIC(CTriangleDialog, CDialog)

CTriangleDialog::CTriangleDialog(CWnd* pParent /*=NULL*/)
    : CDialog(CTriangleDialog::IDD, pParent)
    , m_strAx(_T("0")), m_strAy(_T("0"))
    , m_strBx(_T("4")), m_strBy(_T("0"))
    , m_strCx(_T("2")), m_strCy(_T("3"))
    , m_strLabelA(_T("A")), m_strLabelB(_T("B")), m_strLabelC(_T("C"))
    , m_strLabelSideA(_T("a")), m_strLabelSideB(_T("b")), m_strLabelSideC(_T("c"))
    , m_strLabelAngleA(_T("\u03B1")), m_strLabelAngleB(_T("\u03B2")), m_strLabelAngleC(_T("\u03B3"))
    , m_bShowVertexLabels(TRUE), m_bShowSideLabels(TRUE)
    , m_bShowAngleLabels(TRUE), m_bShowAngleValues(FALSE)
    , m_bCircumcircle(FALSE), m_bIncircle(FALSE)
    , m_bPerpBisectors(FALSE), m_bAngleBisectors(FALSE)
    , m_bAltitudes(FALSE), m_bMedians(FALSE)
    , m_nType(0)
    , m_strLabel(_T(""))
{
}

CTriangleDialog::~CTriangleDialog()
{
}

void CTriangleDialog::PrePopulate(const GeoTriangle& tri)
{
    m_strAx.Format(_T("%.4g"), tri.ax); m_strAy.Format(_T("%.4g"), tri.ay);
    m_strBx.Format(_T("%.4g"), tri.bx); m_strBy.Format(_T("%.4g"), tri.by);
    m_strCx.Format(_T("%.4g"), tri.cx); m_strCy.Format(_T("%.4g"), tri.cy);

    m_strLabelA = tri.labelA; m_strLabelB = tri.labelB; m_strLabelC = tri.labelC;
    m_strLabelSideA = tri.labelSideA; m_strLabelSideB = tri.labelSideB; m_strLabelSideC = tri.labelSideC;
    m_strLabelAngleA = tri.labelAngleA; m_strLabelAngleB = tri.labelAngleB; m_strLabelAngleC = tri.labelAngleC;

    m_bShowVertexLabels = tri.bShowVertexLabels;
    m_bShowSideLabels   = tri.bShowSideLabels;
    m_bShowAngleLabels  = tri.bShowAngleLabels;
    m_bShowAngleValues  = tri.bShowAngleValues;
    m_bCircumcircle     = tri.bCircumcircle;
    m_bIncircle         = tri.bIncircle;
    m_bPerpBisectors    = tri.bPerpBisectors;
    m_bAngleBisectors   = tri.bAngleBisectors;
    m_bAltitudes        = tri.bAltitudes;
    m_bMedians          = tri.bMedians;
    m_nType             = (int)tri.type;
    m_strLabel          = tri.label;
}

void CTriangleDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TRI_AX, m_strAx); DDX_Text(pDX, IDC_TRI_AY, m_strAy);
    DDX_Text(pDX, IDC_TRI_BX, m_strBx); DDX_Text(pDX, IDC_TRI_BY, m_strBy);
    DDX_Text(pDX, IDC_TRI_CX, m_strCx); DDX_Text(pDX, IDC_TRI_CY, m_strCy);

    DDX_Text(pDX, IDC_TRI_LABEL_A,  m_strLabelA);
    DDX_Text(pDX, IDC_TRI_LABEL_B,  m_strLabelB);
    DDX_Text(pDX, IDC_TRI_LABEL_C,  m_strLabelC);
    DDX_Text(pDX, IDC_TRI_LABEL_SIDEA, m_strLabelSideA);
    DDX_Text(pDX, IDC_TRI_LABEL_SIDEB, m_strLabelSideB);
    DDX_Text(pDX, IDC_TRI_LABEL_SIDEC, m_strLabelSideC);
    DDX_Text(pDX, IDC_TRI_LABEL_ANGLEA, m_strLabelAngleA);
    DDX_Text(pDX, IDC_TRI_LABEL_ANGLEB, m_strLabelAngleB);
    DDX_Text(pDX, IDC_TRI_LABEL_ANGLEC, m_strLabelAngleC);

    DDX_Check(pDX, IDC_TRI_SHOW_VERTLABELS, m_bShowVertexLabels);
    DDX_Check(pDX, IDC_TRI_SHOW_SIDELABELS, m_bShowSideLabels);
    DDX_Check(pDX, IDC_TRI_SHOW_ANGLABELS,  m_bShowAngleLabels);
    DDX_Check(pDX, IDC_TRI_SHOW_ANGLVALUES, m_bShowAngleValues);
    DDX_Check(pDX, IDC_TRI_CIRCUMCIRCLE,    m_bCircumcircle);
    DDX_Check(pDX, IDC_TRI_INCIRCLE,        m_bIncircle);
    DDX_Check(pDX, IDC_TRI_PERP_BISECTORS,  m_bPerpBisectors);
    DDX_Check(pDX, IDC_TRI_ANGLE_BISECTORS, m_bAngleBisectors);
    DDX_Check(pDX, IDC_TRI_ALTITUDES,       m_bAltitudes);
    DDX_Check(pDX, IDC_TRI_MEDIANS,         m_bMedians);

    DDX_Radio(pDX, IDC_TRI_TYPE_GENERAL, m_nType);

    DDX_Text(pDX, IDC_TRI_LABEL, m_strLabel);
}

BEGIN_MESSAGE_MAP(CTriangleDialog, CDialog)
    ON_BN_CLICKED(IDC_TRI_TYPE_GENERAL,     &CTriangleDialog::OnClickedTypeGeneral)
    ON_BN_CLICKED(IDC_TRI_TYPE_RIGHT,       &CTriangleDialog::OnClickedTypeRight)
    ON_BN_CLICKED(IDC_TRI_TYPE_ISOSCELES,   &CTriangleDialog::OnClickedTypeIsosceles)
    ON_BN_CLICKED(IDC_TRI_TYPE_EQUILATERAL, &CTriangleDialog::OnClickedTypeEquilateral)
END_MESSAGE_MAP()

BOOL CTriangleDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the radio button according to m_nType
    CheckRadioButton(IDC_TRI_TYPE_GENERAL, IDC_TRI_TYPE_EQUILATERAL,
                     IDC_TRI_TYPE_GENERAL + m_nType);
    return TRUE;
}

// ---------------------------------------------------------------------------
// Helper: parse a CString to double
// ---------------------------------------------------------------------------
static double ParseDbl(const CString& s, double def)
{
    double v = def;
    _stscanf_s(s, _T("%lf"), &v);
    return v;
}

// ---------------------------------------------------------------------------
// UpdateCFromType — auto-compute C from A and B based on type
// ---------------------------------------------------------------------------
void CTriangleDialog::UpdateCFromType()
{
    // Read current A and B
    CString sAx, sAy, sBx, sBy;
    GetDlgItemText(IDC_TRI_AX, sAx); GetDlgItemText(IDC_TRI_AY, sAy);
    GetDlgItemText(IDC_TRI_BX, sBx); GetDlgItemText(IDC_TRI_BY, sBy);

    double ax = ParseDbl(sAx, 0.0), ay = ParseDbl(sAy, 0.0);
    double bx = ParseDbl(sBx, 4.0), by = ParseDbl(sBy, 0.0);

    double mx = (ax + bx) * 0.5;
    double my = (ay + by) * 0.5;
    double ab = std::sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay));

    // Perpendicular direction to AB (normalized)
    double dx = bx - ax, dy = by - ay;
    double len = ab > 1e-10 ? ab : 1.0;
    double nx = -dy / len, ny = dx / len;  // perpendicular (rotated 90° CCW)

    double cx = mx, cy = my;

    switch (m_nType)
    {
    case TT_RIGHT_C:
        // Right angle at C: C is on circle with diameter AB.
        // C = midpoint(A,B) + perpendicular * (ab/2)
        cx = mx + nx * (ab * 0.5);
        cy = my + ny * (ab * 0.5);
        break;

    case TT_ISOSCELES:
        // Isosceles AC = BC: C on perpendicular bisector, height = ab*0.75
        cx = mx + nx * (ab * 0.75);
        cy = my + ny * (ab * 0.75);
        break;

    case TT_EQUILATERAL:
        // Equilateral: height = ab * sqrt(3)/2
        cx = mx + nx * (ab * std::sqrt(3.0) / 2.0);
        cy = my + ny * (ab * std::sqrt(3.0) / 2.0);
        break;

    default:
        return;  // General: don't modify C
    }

    CString sx, sy;
    sx.Format(_T("%.4g"), cx);
    sy.Format(_T("%.4g"), cy);
    SetDlgItemText(IDC_TRI_CX, sx);
    SetDlgItemText(IDC_TRI_CY, sy);
}

void CTriangleDialog::OnClickedTypeGeneral()
{
    m_nType = TT_GENERAL;
    // Don't auto-change C for general
}

void CTriangleDialog::OnClickedTypeRight()
{
    m_nType = TT_RIGHT_C;
    UpdateCFromType();
}

void CTriangleDialog::OnClickedTypeIsosceles()
{
    m_nType = TT_ISOSCELES;
    UpdateCFromType();
}

void CTriangleDialog::OnClickedTypeEquilateral()
{
    m_nType = TT_EQUILATERAL;
    UpdateCFromType();
}

void CTriangleDialog::OnOK()
{
    UpdateData(TRUE);

    m_result.ax = ParseDbl(m_strAx, 0.0); m_result.ay = ParseDbl(m_strAy, 0.0);
    m_result.bx = ParseDbl(m_strBx, 4.0); m_result.by = ParseDbl(m_strBy, 0.0);
    m_result.cx = ParseDbl(m_strCx, 2.0); m_result.cy = ParseDbl(m_strCy, 3.0);

    m_result.labelA = m_strLabelA; m_result.labelB = m_strLabelB; m_result.labelC = m_strLabelC;
    m_result.labelSideA = m_strLabelSideA; m_result.labelSideB = m_strLabelSideB; m_result.labelSideC = m_strLabelSideC;
    m_result.labelAngleA = m_strLabelAngleA; m_result.labelAngleB = m_strLabelAngleB; m_result.labelAngleC = m_strLabelAngleC;

    m_result.bShowVertexLabels = m_bShowVertexLabels;
    m_result.bShowSideLabels   = m_bShowSideLabels;
    m_result.bShowAngleLabels  = m_bShowAngleLabels;
    m_result.bShowAngleValues  = m_bShowAngleValues;
    m_result.bCircumcircle     = m_bCircumcircle;
    m_result.bIncircle         = m_bIncircle;
    m_result.bPerpBisectors    = m_bPerpBisectors;
    m_result.bAngleBisectors   = m_bAngleBisectors;
    m_result.bAltitudes        = m_bAltitudes;
    m_result.bMedians          = m_bMedians;
    m_result.type              = (TriangleType)m_nType;
    m_result.label             = m_strLabel;

    CDialog::OnOK();
}
