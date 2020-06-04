
// GraphDrawerDoc.h : interface of the CGraphDrawerDoc class
//
#include "DrawOptionsDialog.h"

#pragma once
struct DrawOptionsData;
	
class CGraphDrawerDoc : public CDocument
{
protected: // create from serialization only
	CGraphDrawerDoc();
	DECLARE_DYNCREATE(CGraphDrawerDoc)

// Attributes
public:
	BOOL m_bShowCoordinateAxes;
DrawOptionsData m_DrawOptionsData;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CGraphDrawerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnDrawDrawoptions();
	// Coordinate variables
	BOOL m_bShowTicks;
	BOOL m_bShowTicksLabel;
	int m_nTicksInterval;
	COLORREF m_clrAxesColor;
	int m_nAxesPenWidth;
	// Draw Sine
	BOOL m_bDrawSine;
	// Draw Cosine
	BOOL m_bDrawCosine;
	// Draw Tangent
	BOOL m_bDrawTan;
	// Draw Cotangent
	BOOL m_bDrawCotan;
	// Draw Exponent function
	BOOL m_bDrawExp;
	// Draw Natural Lograrithm
	BOOL m_bDrawLN;
	// Draw Arcus Sine
	BOOL m_bDrawArcsine;
protected:
	CSize m_sizDoc;
public:
	CSize& GetDocSize(void);
	BOOL m_bDrawArccosine;
	BOOL m_bDrawArctan;
	BOOL m_bDrawArccotan;
	BOOL m_bDrawHyperbolicSine;
	BOOL m_bDrawHyperbolicCosine;
	BOOL m_bDrawHyperbolicTan;
	BOOL m_bDrawHyperbolicCotan;
	COLORREF m_clrBkgndColor;
};

void DrawCoordinateAxes(CDC* pDC, BOOL bShowTicks, int nTicksInterval,
						BOOL bShowLabels,CRect m_rcPrintRect);

void DrawSine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrSineColor);

void DrawCosine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrCosineColor);

void DrawTan( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrTanColor);

void DrawCotan( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrCotanColor );

void DrawExp( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrExpColor);

void DrawLN( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrLNColor);

void DrawArcsine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrArcsineColor);

void DrawArccosine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrArccosineColor);

void DrawArctan ( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrArctanColor);

double cotan( double x);

double arccotan( double y);

void DrawArccotan ( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrArccotanColor);

void DrawHyperbolicSine ( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrHyperbolicSine);

void DrawHyperbolicCosine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrHyperbolicCosine);

void DrawHyperbolicTan( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrHyperbolicTan);

double coth(double x);

void DrawHyperbolicCotan( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrHyperbolicCotan);
