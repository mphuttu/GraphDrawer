#pragma once
#include "GeometricObjects.h"

// CLineSegmentDialog — modal dialog for adding / editing a line segment.

class CLineSegmentDialog : public CDialog
{
    DECLARE_DYNAMIC(CLineSegmentDialog)

public:
    CLineSegmentDialog(CWnd* pParent = NULL);
    virtual ~CLineSegmentDialog();

    enum { IDD = IDD_LINEDIALOG };

    // Pre-populate fields from an existing segment (call before DoModal for Edit).
    void PrePopulate(const GeoLineSegment& seg);

    // Retrieve the result after DoModal() == IDOK.
    const GeoLineSegment& GetResult() const { return m_result; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;

    DECLARE_MESSAGE_MAP()

private:
    GeoLineSegment m_result;

    // DDX members
    CString m_strX1, m_strY1, m_strX2, m_strY2;
    CString m_strLabel;
    BOOL    m_bParallel;
    CString m_strParallelDist;
    BOOL    m_bTransversal;
    CString m_strTransX, m_strTransY, m_strTransAngle;

    void UpdateControlStates();

    afx_msg void OnClickedParallelCheck();
    afx_msg void OnClickedTransversalCheck();
};
