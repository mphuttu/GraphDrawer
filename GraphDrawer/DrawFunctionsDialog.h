#pragma once


// CDrawFunctionsDialog dialog

class CDrawFunctionsDialog : public CDialog
{
	DECLARE_DYNAMIC(CDrawFunctionsDialog)

public:
	CDrawFunctionsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDrawFunctionsDialog();

// Dialog Data
	enum { IDD = IDD_DRAWFUNCTIONSDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bDrawSine;
	afx_msg void OnClickedDrawsinecheck();
	BOOL m_bDrawCosine;
	afx_msg void OnClickedDrawcosinecheck();
	BOOL m_bDrawTan;
	afx_msg void OnClickedDrawtancheck();
	BOOL m_bDrawCotan;
	afx_msg void OnClickedDrawcotancheck();
	afx_msg void OnClickedDrawexpcheck();
	BOOL m_bDrawExp;
	BOOL m_bDrawLN;
	afx_msg void OnClickedDrawlncheck();
	BOOL m_bDrawArcsine;
	afx_msg void OnClickedDrawarcsinecheck();
	BOOL m_bDrawArccosine;
	afx_msg void OnClickedDrawarccosinecheck();
	BOOL m_bDrawArctan;
	afx_msg void OnClickedDrawarctancheck();
	BOOL m_bDrawArccotan;
	afx_msg void OnClickedDrawarccotancheck();
	BOOL m_bDrawHyperbolicSine;
	afx_msg void OnClickedDrawhyperbolicsinecheck();
	BOOL m_bDrawHyperbolicCosine;
	afx_msg void OnClickedDrawhyperboliccosinecheck();
	BOOL m_bDrawHyperbolicTan;
	afx_msg void OnClickedDrawhyperbolictangentcheck();
	BOOL m_bDrawHyperbolicCotan;
	afx_msg void OnClickedDrawhyperboliccotangentcheck();
};
