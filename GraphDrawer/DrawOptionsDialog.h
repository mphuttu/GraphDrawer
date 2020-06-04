#pragma once
#include "afxwin.h"
#include "afxcolorbutton.h"
#include "afxcmn.h"


// CDrawOptionsDialog dialog
struct DrawOptionsData
{
	// Check Boxes
	BOOL m_bCheckStateShowCoordinateAxes;
	BOOL m_bCheckStateShowTicks;
	BOOL m_bCheckStateShowTicksLabel;

	// Edit Boxes
	int m_nTicksInterval;

	// Combo Boxes index
	// CMFCColorButton
	COLORREF clrCoordColor;
	int nIndexThickness;
	CString strCoordAxesThickness;

	// Background color
	COLORREF clrBkgndColor;
};

class CDrawOptionsDialog : public CDialog
{
	DECLARE_DYNAMIC(CDrawOptionsDialog)

public:
	CDrawOptionsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDrawOptionsDialog();

// Dialog Data
	enum { IDD = IDD_DRAWOPTIONSDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bShowCoordinateAxes;
	afx_msg void OnClickedShowcoordinateaxescheck();
//	int m_nMaxX;
//	int m_nMaxY;
	BOOL m_bShowTicks;
	afx_msg void OnClickedShowtickscheck();
	afx_msg void OnClickedShowtickslabelcheck();
	BOOL m_bShowTicksLabel;
	afx_msg void OnBnClickedOk();
	int m_nTicksInterval;
	virtual BOOL OnInitDialog();
	CEdit m_editTicksInterval;
	CMFCColorButton m_wndCoordAxesColor;
	COLORREF m_clrAxesColor;
	int  m_nAxesPenWidth;
	void PopulatePenWidthCombo(void);
	afx_msg void OnClickedMfccolorbuttoncoord();
	void SetData(DrawOptionsData dr);
	void GetData(DrawOptionsData* dr);
	CComboBox m_cbPenWidth;
	afx_msg void OnSelchangePenwidthcombo();
	CMFCColorButton m_wndBkgndColor;
	COLORREF m_clrBkgndColor;
	afx_msg void OnClickedBkgndmfccolorbutton();
};
