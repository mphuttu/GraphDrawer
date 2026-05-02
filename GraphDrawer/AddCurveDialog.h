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

	// Pre-populate all fields from an existing curve (call before DoModal for Edit).
	void PrePopulate(const UserCurve& curve);

private:
	CString   m_strCaption;   // if non-empty, overrides dialog caption in OnInitDialog

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
	CString m_strXFrom;
	CString m_strXTo;
	// Parametric mode
	CString m_strExprParX;
	CString m_strExprParY;
	CString m_strTFrom;
	CString m_strTTo;
	// Polar mode
	CString m_strExprPolar;
	CString m_strPhiFrom;
	CString m_strPhiTo;
	// Implicit mode
	CString m_strExprImplicit;
	CString m_strXFromImp;
	CString m_strXToImp;
	CString m_strYFromImp;
	CString m_strYToImp;

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
