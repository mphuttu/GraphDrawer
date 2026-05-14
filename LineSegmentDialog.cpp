// LineSegmentDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDrawer.h"
#include "LineSegmentDialog.h"
#include "afxdialogex.h"

// CLineSegmentDialog dialog

IMPLEMENT_DYNAMIC(CLineSegmentDialog, CDialog)

CLineSegmentDialog::CLineSegmentDialog(CWnd* pParent /*=NULL*/)
    : CDialog(CLineSegmentDialog::IDD, pParent)
    , m_strX1(_T("0")), m_strY1(_T("0"))
    , m_strX2(_T("4")), m_strY2(_T("0"))
    , m_strLabel(_T(""))
    , m_bParallel(FALSE), m_strParallelDist(_T("2"))
    , m_bTransversal(FALSE)
    , m_strTransX(_T("2")), m_strTransY(_T("0")), m_strTransAngle(_T("60"))
{
}

CLineSegmentDialog::~CLineSegmentDialog()
{
}

void CLineSegmentDialog::PrePopulate(const GeoLineSegment& seg)
{
    m_strX1.Format(_T("%.4g"), seg.x1);
    m_strY1.Format(_T("%.4g"), seg.y1);
    m_strX2.Format(_T("%.4g"), seg.x2);
    m_strY2.Format(_T("%.4g"), seg.y2);
    m_strLabel    = seg.label;
    m_bParallel   = seg.bParallel;
    m_strParallelDist.Format(_T("%.4g"), seg.dParallelOffset);
    m_bTransversal = seg.bTransversal;
    m_strTransX.Format(_T("%.4g"), seg.dTransversalX);
    m_strTransY.Format(_T("%.4g"), seg.dTransversalY);
    m_strTransAngle.Format(_T("%.4g"), seg.dTransversalAngleDeg);
}

void CLineSegmentDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_LINE_X1,              m_strX1);
    DDX_Text(pDX, IDC_LINE_Y1,              m_strY1);
    DDX_Text(pDX, IDC_LINE_X2,              m_strX2);
    DDX_Text(pDX, IDC_LINE_Y2,              m_strY2);
    DDX_Text(pDX, IDC_LINE_LABEL,           m_strLabel);
    DDX_Check(pDX, IDC_LINE_PARALLEL_CHECK, m_bParallel);
    DDX_Text(pDX, IDC_LINE_PARALLEL_DIST,   m_strParallelDist);
    DDX_Check(pDX, IDC_LINE_TRANSVERSAL_CHECK, m_bTransversal);
    DDX_Text(pDX, IDC_LINE_TRANS_X,         m_strTransX);
    DDX_Text(pDX, IDC_LINE_TRANS_Y,         m_strTransY);
    DDX_Text(pDX, IDC_LINE_TRANS_ANGLE,     m_strTransAngle);
}

BEGIN_MESSAGE_MAP(CLineSegmentDialog, CDialog)
    ON_BN_CLICKED(IDC_LINE_PARALLEL_CHECK,    &CLineSegmentDialog::OnClickedParallelCheck)
    ON_BN_CLICKED(IDC_LINE_TRANSVERSAL_CHECK, &CLineSegmentDialog::OnClickedTransversalCheck)
END_MESSAGE_MAP()

BOOL CLineSegmentDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    UpdateControlStates();
    return TRUE;
}

void CLineSegmentDialog::UpdateControlStates()
{
    BOOL bPar  = (IsDlgButtonChecked(IDC_LINE_PARALLEL_CHECK)    == BST_CHECKED);
    BOOL bTrans = (IsDlgButtonChecked(IDC_LINE_TRANSVERSAL_CHECK) == BST_CHECKED);

    GetDlgItem(IDC_LINE_PARALLEL_DIST)->EnableWindow(bPar);
    GetDlgItem(IDC_LINE_TRANS_X)->EnableWindow(bTrans);
    GetDlgItem(IDC_LINE_TRANS_Y)->EnableWindow(bTrans);
    GetDlgItem(IDC_LINE_TRANS_ANGLE)->EnableWindow(bTrans);
}

void CLineSegmentDialog::OnClickedParallelCheck()
{
    UpdateControlStates();
}

void CLineSegmentDialog::OnClickedTransversalCheck()
{
    UpdateControlStates();
}

void CLineSegmentDialog::OnOK()
{
    UpdateData(TRUE);

    auto parseDouble = [](const CString& s, double def) -> double {
        double v = def;
        if (_stscanf_s(s, _T("%lf"), &v) != 1) v = def;
        return v;
    };

    m_result.x1 = parseDouble(m_strX1, 0.0);
    m_result.y1 = parseDouble(m_strY1, 0.0);
    m_result.x2 = parseDouble(m_strX2, 4.0);
    m_result.y2 = parseDouble(m_strY2, 0.0);
    m_result.label          = m_strLabel;
    m_result.bParallel      = m_bParallel;
    m_result.dParallelOffset = parseDouble(m_strParallelDist, 2.0);
    m_result.bTransversal   = m_bTransversal;
    m_result.dTransversalX  = parseDouble(m_strTransX, 2.0);
    m_result.dTransversalY  = parseDouble(m_strTransY, 0.0);
    m_result.dTransversalAngleDeg = parseDouble(m_strTransAngle, 60.0);

    CDialog::OnOK();
}
