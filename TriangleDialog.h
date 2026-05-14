#pragma once
#include "GeometricObjects.h"

// CTriangleDialog — modal dialog for adding / editing a triangle.

class CTriangleDialog : public CDialog
{
    DECLARE_DYNAMIC(CTriangleDialog)

public:
    CTriangleDialog(CWnd* pParent = NULL);
    virtual ~CTriangleDialog();

    enum { IDD = IDD_TRIANGLEDIALOG };

    // Pre-populate fields from an existing triangle (call before DoModal for Edit).
    void PrePopulate(const GeoTriangle& tri);

    // Retrieve the result after DoModal() == IDOK.
    const GeoTriangle& GetResult() const { return m_result; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;

    DECLARE_MESSAGE_MAP()

private:
    GeoTriangle m_result;

    // DDX members — vertex coordinates
    CString m_strAx, m_strAy;
    CString m_strBx, m_strBy;
    CString m_strCx, m_strCy;

    // Labels
    CString m_strLabelA, m_strLabelB, m_strLabelC;
    CString m_strLabelSideA, m_strLabelSideB, m_strLabelSideC;
    CString m_strLabelAngleA, m_strLabelAngleB, m_strLabelAngleC;

    // Display options
    BOOL m_bShowVertexLabels;
    BOOL m_bShowSideLabels;
    BOOL m_bShowAngleLabels;
    BOOL m_bShowAngleValues;
    BOOL m_bCircumcircle;
    BOOL m_bIncircle;
    BOOL m_bPerpBisectors;
    BOOL m_bAngleBisectors;
    BOOL m_bAltitudes;
    BOOL m_bMedians;

    // Type
    int  m_nType;  // 0=General, 1=Right at C, 2=Isosceles, 3=Equilateral

    // Overall label
    CString m_strLabel;

    // Apply type defaults: auto-compute C given A and B
    void ApplyTypeDefaults();
    void UpdateCFromType();

    afx_msg void OnClickedTypeGeneral();
    afx_msg void OnClickedTypeRight();
    afx_msg void OnClickedTypeIsosceles();
    afx_msg void OnClickedTypeEquilateral();
};
