#pragma once
#include "afxcolorbutton.h"
#include "GraphDrawerDoc.h"   // for UserCurve, IDD_ADDCURVEDIALOG


class CAddCurveDialog : public CDialog
{
	DECLARE_DYNAMIC(CAddCurveDialog)

public:
	CAddCurveDialog(CWnd* pParent = NULL);
	virtual ~CAddCurveDialog();

	enum { IDD = IDD_ADDCURVEDIALOG };

	// The fully-populated curve to retrieve after DoModal() returns IDOK.
	const UserCurve& GetResult() const { return m_result; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

public:
	// Data members bound by DDX
	int     m_nCurveType;      // 0 = y=f(x), 1 = parametric, 2 = polar
	CString m_strLabel;
	// y = f(x) mode
	CString m_strExprYFX;
	double  m_dXFrom;
	double  m_dXTo;
	// Parametric mode
	CString m_strExprParX;
	CString m_strExprParY;
	double  m_dTFrom;
	double  m_dTTo;
	// Polar mode
	CString m_strExprPolar;
	double  m_dPhiFrom;
	double  m_dPhiTo;
	// Implicit mode
	CString m_strExprImplicit;
	double  m_dXFromImp = -2.0, m_dXToImp = 2.0;
	double  m_dYFromImp = -2.0, m_dYToImp = 2.0;

	COLORREF        m_color;
	CMFCColorButton m_wndColor;

private:
	UserCurve m_result;

	void UpdateControlVisibility();

	afx_msg void OnClickedYFX();
	afx_msg void OnClickedParam();
	afx_msg void OnClickedPolar();
	afx_msg void OnClickedImplicit();
};
