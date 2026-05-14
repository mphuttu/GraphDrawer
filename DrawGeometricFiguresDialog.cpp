// DrawGeometricFiguresDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GraphDrawer.h"
#include "DrawGeometricFiguresDialog.h"
#include "afxdialogex.h"
#include <algorithm>

#include "MainFrm.h"
#include "GraphDrawerDoc.h"
#include "LineSegmentDialog.h"
#include "TriangleDialog.h"

// CDrawGeometricFiguresDialog dialog

IMPLEMENT_DYNAMIC(CDrawGeometricFiguresDialog, CDialog)

CDrawGeometricFiguresDialog::CDrawGeometricFiguresDialog(CWnd* pParent /*=NULL*/)
    : CDialog(CDrawGeometricFiguresDialog::IDD, pParent)
{
}

CDrawGeometricFiguresDialog::~CDrawGeometricFiguresDialog()
{
}

void CDrawGeometricFiguresDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GEOLIST, m_listGeo);
}

BEGIN_MESSAGE_MAP(CDrawGeometricFiguresDialog, CDialog)
    ON_BN_CLICKED(IDC_ADDLINE_BUTTON,      &CDrawGeometricFiguresDialog::OnClickedAddLine)
    ON_BN_CLICKED(IDC_ADDTRIANGLE_BUTTON,  &CDrawGeometricFiguresDialog::OnClickedAddTriangle)
    ON_BN_CLICKED(IDC_GEOEDIT_BUTTON,      &CDrawGeometricFiguresDialog::OnClickedEdit)
    ON_BN_CLICKED(IDC_GEOREMOVE_BUTTON,    &CDrawGeometricFiguresDialog::OnClickedRemove)
    ON_BN_CLICKED(IDC_GEOTOGGLE_BUTTON,    &CDrawGeometricFiguresDialog::OnClickedToggle)
    ON_LBN_DBLCLK(IDC_GEOLIST,            &CDrawGeometricFiguresDialog::OnDblclkList)
END_MESSAGE_MAP()

// ---------------------------------------------------------------------------
// Modeless overrides — do not destroy window on OK/Cancel
// ---------------------------------------------------------------------------
void CDrawGeometricFiguresDialog::OnOK()
{
    ShowWindow(SW_HIDE);
}

void CDrawGeometricFiguresDialog::OnCancel()
{
    ShowWindow(SW_HIDE);
}

BOOL CDrawGeometricFiguresDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    RefreshList();
    return TRUE;
}

// ---------------------------------------------------------------------------
// Helper: get doc safely
// ---------------------------------------------------------------------------
static CGraphDrawerDoc* GetDoc()
{
    CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
    if (!pFrame) return nullptr;
    return dynamic_cast<CGraphDrawerDoc*>(pFrame->GetActiveDocument());
}

// ---------------------------------------------------------------------------
// RefreshList — rebuild the list box from the document's geo-object vector
// ---------------------------------------------------------------------------
void CDrawGeometricFiguresDialog::RefreshList()
{
    m_listGeo.ResetContent();
    CGraphDrawerDoc* pDoc = GetDoc();
    if (!pDoc) return;

    CSingleLock lock(&pDoc->m_csGeoObjects, TRUE);
    for (int i = 0; i < (int)pDoc->m_vecGeoObjects.size(); ++i)
    {
        const GeoObject& obj = pDoc->m_vecGeoObjects[i];
        CString entry = obj.GetDisplayName();
        if (!obj.IsVisible())
            entry += _T(" [hidden]");
        m_listGeo.AddString(entry);
    }
}

// ---------------------------------------------------------------------------
// Button handlers
// ---------------------------------------------------------------------------
void CDrawGeometricFiguresDialog::OnClickedAddLine()
{
    CLineSegmentDialog dlg(this);
    if (dlg.DoModal() == IDOK)
    {
        CGraphDrawerDoc* pDoc = GetDoc();
        if (!pDoc) return;

        GeoObject obj;
        obj.type = GOT_LINE_SEGMENT;
        obj.line = dlg.GetResult();
        pDoc->AddGeoObject(obj);
        RefreshList();
    }
}

void CDrawGeometricFiguresDialog::OnClickedAddTriangle()
{
    CTriangleDialog dlg(this);
    if (dlg.DoModal() == IDOK)
    {
        CGraphDrawerDoc* pDoc = GetDoc();
        if (!pDoc) return;

        GeoObject obj;
        obj.type = GOT_TRIANGLE;
        obj.triangle = dlg.GetResult();
        pDoc->AddGeoObject(obj);
        RefreshList();
    }
}

void CDrawGeometricFiguresDialog::OnClickedEdit()
{
    int sel = m_listGeo.GetCurSel();
    if (sel == LB_ERR) return;

    CGraphDrawerDoc* pDoc = GetDoc();
    if (!pDoc) return;

    GeoObject obj;
    {
        CSingleLock lock(&pDoc->m_csGeoObjects, TRUE);
        if (sel >= (int)pDoc->m_vecGeoObjects.size()) return;
        obj = pDoc->m_vecGeoObjects[sel];
    }

    if (obj.type == GOT_LINE_SEGMENT)
    {
        CLineSegmentDialog dlg(this);
        dlg.PrePopulate(obj.line);
        if (dlg.DoModal() == IDOK)
        {
            obj.line = dlg.GetResult();
            pDoc->ReplaceGeoObject(sel, obj);
            RefreshList();
        }
    }
    else // GOT_TRIANGLE
    {
        CTriangleDialog dlg(this);
        dlg.PrePopulate(obj.triangle);
        if (dlg.DoModal() == IDOK)
        {
            obj.triangle = dlg.GetResult();
            pDoc->ReplaceGeoObject(sel, obj);
            RefreshList();
        }
    }
}

void CDrawGeometricFiguresDialog::OnClickedRemove()
{
    int sel = m_listGeo.GetCurSel();
    if (sel == LB_ERR) return;

    CGraphDrawerDoc* pDoc = GetDoc();
    if (!pDoc) return;

    pDoc->RemoveGeoObject(sel);
    RefreshList();

    // Restore selection near the removed item
    int count = m_listGeo.GetCount();
    if (count > 0)
        m_listGeo.SetCurSel(std::min(sel, count - 1));
}

void CDrawGeometricFiguresDialog::OnClickedToggle()
{
    int sel = m_listGeo.GetCurSel();
    if (sel == LB_ERR) return;

    CGraphDrawerDoc* pDoc = GetDoc();
    if (!pDoc) return;

    {
        CSingleLock lock(&pDoc->m_csGeoObjects, TRUE);
        if (sel >= (int)pDoc->m_vecGeoObjects.size()) return;
        GeoObject& obj = pDoc->m_vecGeoObjects[sel];
        obj.SetVisible(!obj.IsVisible());
    }
    pDoc->UpdateAllViews(NULL);
    RefreshList();
    m_listGeo.SetCurSel(sel);
}

void CDrawGeometricFiguresDialog::OnDblclkList()
{
    OnClickedEdit();
}
