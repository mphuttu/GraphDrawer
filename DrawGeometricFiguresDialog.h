#pragma once
#include "GeometricObjects.h"

// CDrawGeometricFiguresDialog — modeless palette for managing geometric figures.

class CDrawGeometricFiguresDialog : public CDialog
{
    DECLARE_DYNAMIC(CDrawGeometricFiguresDialog)

public:
    CDrawGeometricFiguresDialog(CWnd* pParent = NULL);
    virtual ~CDrawGeometricFiguresDialog();

    enum { IDD = IDD_DRAWGEOMETRICFIGURESDIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;
    virtual void OnCancel() override;

    DECLARE_MESSAGE_MAP()

public:
    // Refresh the list box from the document.
    void RefreshList();

private:
    CListBox m_listGeo;

    afx_msg void OnClickedAddLine();
    afx_msg void OnClickedAddTriangle();
    afx_msg void OnClickedEdit();
    afx_msg void OnClickedRemove();
    afx_msg void OnClickedToggle();
    afx_msg void OnDblclkList();
};
