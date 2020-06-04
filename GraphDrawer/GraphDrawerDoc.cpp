
// GraphDrawerDoc.cpp : implementation of the CGraphDrawerDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GraphDrawer.h"
#endif

#include "GraphDrawerDoc.h"
#include "DrawOptionsDialog.h"
#include "DrawFunctionsDialog.h"

#include <propkey.h>
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGraphDrawerDoc

IMPLEMENT_DYNCREATE(CGraphDrawerDoc, CDocument)

BEGIN_MESSAGE_MAP(CGraphDrawerDoc, CDocument)
	ON_COMMAND(ID_DRAW_DRAWOPTIONS, &CGraphDrawerDoc::OnDrawDrawoptions)
END_MESSAGE_MAP()


// CGraphDrawerDoc construction/destruction

CGraphDrawerDoc::CGraphDrawerDoc() : m_sizDoc(850,1100)
{
	// TODO: add one-time construction code here

	m_nTicksInterval = 100;

	// Coordinate Axes and Ticks
	m_DrawOptionsData.m_bCheckStateShowCoordinateAxes = TRUE;
	m_DrawOptionsData.m_bCheckStateShowTicks = TRUE;
	m_DrawOptionsData.m_bCheckStateShowTicksLabel = FALSE;

	// Coordinate Draw Options
	m_DrawOptionsData.m_nTicksInterval = 100;

	// Coordinate combo boxes
	m_DrawOptionsData.nIndexThickness = 0;
	m_DrawOptionsData.strCoordAxesThickness = _T("1 pt");
	m_DrawOptionsData.clrCoordColor = RGB(0,0, 255);
	
	// Background color
	m_DrawOptionsData.clrBkgndColor = RGB(96, 96, 96);
}

CGraphDrawerDoc::~CGraphDrawerDoc()
{
}

BOOL CGraphDrawerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CGraphDrawerDoc serialization

void CGraphDrawerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CGraphDrawerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CGraphDrawerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CGraphDrawerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CGraphDrawerDoc diagnostics

#ifdef _DEBUG
void CGraphDrawerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGraphDrawerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGraphDrawerDoc commands


void CGraphDrawerDoc::OnDrawDrawoptions()
{
	// TODO: Add your command handler code here
	CDrawOptionsDialog aDlg;

	 aDlg.SetData(m_DrawOptionsData);
	
	if ( aDlg.DoModal() == IDOK )
	{
		m_bShowCoordinateAxes = aDlg.m_bShowCoordinateAxes;
		m_bShowTicks = aDlg.m_bShowTicks;
		m_bShowTicksLabel = aDlg.m_bShowTicksLabel;

		m_nTicksInterval = aDlg.m_nTicksInterval;

		m_clrAxesColor = aDlg.m_clrAxesColor;

		m_nAxesPenWidth  = aDlg.m_nAxesPenWidth;

		// Background color
		m_clrBkgndColor = aDlg.m_clrBkgndColor;
		
	aDlg.GetData(&m_DrawOptionsData);
	}
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
}

void DrawCoordinateAxes(CDC* pDC, BOOL bShowTicks, int nTicksInterval,
						BOOL bShowLabels, CRect m_rcPrintRect)
{
	// Set mapping mode
	// pDC->SetMapMode(MM_LOMETRIC);
	
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;
	
	// SetViewportOrgEx (hDC, nHalfWidth, nHalfHeight, NULL);

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	int nEndTickY =  -(int) (nHalfHeight*0.99);
	int nEndIickX = (int) ( nHalfWidth*0.99);
	
	/*
	CString strHeights;
	strHeights.Format(_T("nHalfWidth is %d and nHalfHeight is %d"), nHalfWidth, nEndTickY);
	AfxMessageBox(strHeights);
	*/

	// For Metric mapping
	// X-Axis
	pDC->MoveTo(ORIGIN.x - nEndIickX, ORIGIN.y);
	pDC->LineTo(ORIGIN.x + nEndIickX, ORIGIN.y);
	// X label
	pDC->TextOutW(ORIGIN.x + (int) (nHalfWidth*0.985), ORIGIN.y +10, _T("X") );

	// Y-Axis
	pDC->MoveTo(ORIGIN.x, ORIGIN.y + nEndTickY);
	pDC->LineTo(ORIGIN.x, ORIGIN.y - nEndTickY);
	// Y label
	pDC->TextOutW(ORIGIN.x +10, ORIGIN.y - (int)( nHalfHeight*0.975), _T("Y"));

	// Ticks
	const int nTickHeightX = 5;
	const int nTickHeightY = 5;
	int nTicksIntervalY = (int)( (double) nHalfHeight* ( (double) nTicksInterval/nHalfWidth) );
	int i=0;
	int j=0;
	const int denominator = nTicksInterval;
	
	

	if ( bShowTicks == TRUE )
	{

		if ( nTicksInterval > 0 && nTicksInterval < nHalfWidth)
		{
			
			
			// X Axis
			
			for ( i = nTicksInterval; i < nHalfWidth -25; i += nTicksInterval) // nHalfWidth -25
			{
				// X ticks, positive X-Axis
				pDC->MoveTo(ORIGIN.x + i, ORIGIN.y - nTickHeightX);
				pDC->LineTo(ORIGIN.x + i, ORIGIN.y + nTickHeightX);
				
				// Show labels
				CString strTemp;
				CString strTempNeg;
				strTemp.Format(_T("%d"), i/denominator);
				strTempNeg.Format(_T("-%d"), i/denominator);
				if ( bShowLabels == TRUE )
					pDC->TextOutW(ORIGIN.x +i, ORIGIN.y -nTickHeightX -10, strTemp);

				// X ticks, negative X-Axis
				pDC->MoveTo(ORIGIN.x -i, ORIGIN.y - nTickHeightX);
				pDC->LineTo(ORIGIN.x -i, ORIGIN.y + nTickHeightX);
				if ( bShowLabels == TRUE )
					pDC->TextOutW(ORIGIN.x -i, ORIGIN.y -nTickHeightX -10, strTempNeg);

				if ( i < nEndTickY)
				{
					// Y ticks, negative Y-Axis
					pDC->MoveTo(ORIGIN.x - nTickHeightY, ORIGIN.y - i);
					pDC->LineTo(ORIGIN.x + nTickHeightY, ORIGIN.y - i);
					// Show labels
					CString strTmp;
					CString strTmpNeg;
					strTmp.Format(_T("%d"), i/denominator);
					strTmpNeg.Format(_T("-%d"), i/denominator);
					if ( bShowLabels == TRUE )
						pDC->TextOutW(ORIGIN.x + nTickHeightY + 7, ORIGIN.y - i, strTmpNeg);

					// Y ticks, positive Y-Axis
					pDC->MoveTo(ORIGIN.x - nTickHeightY, ORIGIN.y + i);
					pDC->LineTo(ORIGIN.x + nTickHeightY, ORIGIN.y + i);
					if ( bShowLabels == TRUE )
						pDC->TextOutW(ORIGIN.x + nTickHeightY + 7, ORIGIN.y + i, strTmp);
				}
			}
			

			// Y Axis
			/*
			for ( j = nTicksInterval; j <  nEndTickY; j += nTicksInterval )
			{
				// Y ticks, positive Y-Axis
				pDC->MoveTo(ORIGIN.x - nTickHeightY, ORIGIN.y - j);
				pDC->LineTo(ORIGIN.x + nTickHeightY, ORIGIN.y - j);
				// Show labels
				CString strTemp;
				strTemp.Format(_T("%d"), j);
				if ( bShowLabels == TRUE )
					pDC->TextOutW(ORIGIN.x + nTickHeightY + 50, ORIGIN.y - j, strTemp);

				// Y ticks, negative Y-Axis
				pDC->MoveTo(ORIGIN.x - nTickHeightY, ORIGIN.y + j);
				pDC->LineTo(ORIGIN.x + nTickHeightY, ORIGIN.y + j);
				if ( bShowLabels == TRUE )
					pDC->TextOutW(ORIGIN.x + nTickHeightY + 50, ORIGIN.y + j, strTemp);
			}
			*/
			

		}
	}
}


CSize& CGraphDrawerDoc::GetDocSize(void)
{
	//TODO: insert return statement here
	return m_sizDoc;
}

void DrawSine( CDC * pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrSineColor)
{
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;

	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;
	
	
	// SetViewportOrgEx (hDC, nHalfWidth, nHalfHeight, NULL);

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	
	// const int NUM = 1000;
	// CPoint SINEAPT [NUM];

	// Does not work
	/*
	for ( i =0; i < NUM; i++) 
			{
				SINEAPT[i].x = ORIGIN.x + i*nHalfWidth / NUM;
				SINEAPT[i].y = (int) (ORIGIN.y + nTicksInterval* sin(TWOPI * i / NUM));

	}

	Polyline( hDC, SINEAPT, NUM);
	*/
	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	int EndX =180*( rcClient.Width() / 2 )/(PI * denominator) -0.05 * denominator;
	/*
	const int kyscale = (int) 90 / deltai;
	const int kxscale = (int) ( nTicksInterval / deltai);
	
	int margin = 50;
	int width = 2 * nHalfWidth - margin;
	int height = 2*nHalfHeight - margin;
	*/

	

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = sin(  i * PI/180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ORIGIN.y + value * denominator;
		int xpos = ORIGIN.x +  i*PI/180 * denominator  ;
		int xposneg = ORIGIN.x - i*PI/180 * denominator;
		int yposneg = ORIGIN.y - value * denominator;
		// Positive x-axis
		pDC->SetPixel(xpos, ypos, clrSineColor);
		// Negative x-axis
		pDC->SetPixel(xposneg, yposneg, clrSineColor);
	}
}

void DrawCosine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrSineColor)
{
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;

	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	int EndX =180*( rcClient.Width() / 2 )/(PI * denominator) -0.05 * denominator;

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = cos(  i * PI/180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ORIGIN.y + value * denominator;
		int xpos = ORIGIN.x +  i*PI/180 * denominator  ;
		int xposneg = ORIGIN.x - i*PI/180 * denominator;
		int yposneg = ORIGIN.y + value * denominator;
		// Positive x-axis
		pDC->SetPixel(xpos, ypos, clrSineColor);
		// Negative x-axis
		pDC->SetPixel(xposneg, yposneg, clrSineColor);
	}

}

void DrawTan( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrTanColor)
{
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;

	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	int EndX =180*( rcClient.Width() / 2 )/(PI * denominator) -0.05 * denominator;

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = tan(  i * PI/180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ORIGIN.y + value * denominator;
		int xpos = ORIGIN.x +  i*PI/180 * denominator  ;
		int xposneg = ORIGIN.x - i*PI/180 * denominator;
		int yposneg = ORIGIN.y - value * denominator;
		// Positive x-axis
		pDC->SetPixel(xpos, ypos, clrTanColor);
		// Negative x-axis
		pDC->SetPixel(xposneg, yposneg, clrTanColor);
	}
}

void DrawCotan( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrCotanColor )
{
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;

	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	int EndX =180*( rcClient.Width() / 2 )/(PI * denominator) -0.05 * denominator;

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = 1 / tan(  i * PI/180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ORIGIN.y + value * denominator;
		int xpos = ORIGIN.x +  i*PI/180 * denominator  ;
		int xposneg = ORIGIN.x - i*PI/180 * denominator;
		int yposneg = ORIGIN.y - value * denominator;
		// Positive x-axis
		pDC->SetPixel(xpos, ypos, clrCotanColor);
		// Negative x-axis
		pDC->SetPixel(xposneg, yposneg, clrCotanColor);
	}
}

void DrawExp( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrExpColor)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	int EndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = exp(i * PI / 180);
		double valueneg = exp(-i * PI / 180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ORIGIN.y + value * denominator;
		int xpos = ORIGIN.x +  i *PI/180 * denominator  ;
		int xposneg = ORIGIN.x - i * PI/180 * denominator;
		int yposneg = ORIGIN.y + valueneg * denominator;
		// Positive x-axis
		pDC->SetPixel(xpos, ypos, clrExpColor);
		// Negative x-axis
		pDC->SetPixel(xposneg, yposneg, clrExpColor);
	}
}

void DrawLN( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrLNColor)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	int EndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = log(i * PI / 180);
		// double valueneg = exp(-i * PI / 180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ORIGIN.y + value * denominator;
		int xpos = ORIGIN.x +  i *PI/180 * denominator  ;
		// int xposneg = ORIGIN.x - i * PI/180 * denominator;
		// int yposneg = ORIGIN.y + valueneg * denominator;
		// Positive x-axis
		pDC->SetPixel(xpos, ypos, clrLNColor);
		// Negative x-axis
		// pDC->SetPixel(xposneg, yposneg, clrLNColor);
	}
}

void DrawArcsine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrArcsineColor)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	double j = 0.0;
	int n = 0;
	double value = 0.0;
	double valueneg = 0.0;
	double valuet = 0.0;
	double valuenegt = 0.0;
	// int EndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = -nHalfHeight - PI*denominator ;
	CString strTemp;
	
	
	
	int nCount = nEndY / (PI*denominator) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);
	
	for ( n = 0; n <= nCount; n++ )
	{
		for ( i = -denominator; i <= denominator; i += deltai )
		{
				value = asin(i * PI/180) + n * 2 * PI;
				valueneg = -value;
				valuet = PI - asin(i * PI / 180 ) + n* 2 * PI;
				valuenegt = -valuet;
			
			// 0.1 mm 
			// 100 * 0,1 mm = 10 mm = 1 cm 
			int ypos = ORIGIN.y + value * denominator;
			int ytpos = ORIGIN.y + valuet * denominator;
			int xpos = ORIGIN.x +  i *PI/180 * denominator  ;
			int xposneg = ORIGIN.x - i * PI/180 * denominator;
			int yposneg = ORIGIN.y + valueneg * denominator;
			int ytposneg = ORIGIN.y + valuenegt * denominator;
			//  Positive x-axis
			if ( xpos > 0 )
			pDC->SetPixel(xpos, ypos, clrArcsineColor);
			// Negative x-axis
			// if ( xposneg < 0 )
			 pDC->SetPixel(xposneg, yposneg, clrArcsineColor);
			// Positive x-axis
			if ( xpos > 0)
			 pDC->SetPixel(xpos, ytpos, clrArcsineColor);
			// Negative x-axis
			//  if ( xposneg < 0)
			 pDC->SetPixel(xposneg, ytposneg, clrArcsineColor);

		}
		
	}

}

void DrawArccosine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrArccosineColor)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	double j = 0.0;
	int n = 0;
	double value = 0.0;
	double valueneg = 0.0;
	double valuet = 0.0;
	double valuenegt = 0.0;
	// int EndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = -nHalfHeight -0.7 * denominator ;
	CString strTemp;
	
	
	
	int nCount = nEndY / (PI*denominator) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);
	
	for ( n = 0; n <= nCount; n++ )
	{
		for ( i = -denominator; i <= denominator; i += deltai )
		{
				value = acos(i * PI/180) + n * 2 * PI;
				valueneg = -value;
				valuet = - acos(i * PI / 180 ) + n* 2 * PI;
				valuenegt = -valuet;
			
			// 0.1 mm 
			// 100 * 0,1 mm = 10 mm = 1 cm 
			int ypos = ORIGIN.y + value * denominator;
			int ytpos = ORIGIN.y + valuet * denominator;
			int xpos = ORIGIN.x +  i *PI/180 * denominator  ;
			int xposneg = ORIGIN.x - i * PI/180 * denominator;
			int yposneg = ORIGIN.y + valueneg * denominator;
			int ytposneg = ORIGIN.y + valuenegt * denominator;
			//  Positive x-axis
			pDC->SetPixel(xpos, ypos, clrArccosineColor);
			// Negative x-axis
			 pDC->SetPixel(xpos, yposneg, clrArccosineColor);
			// Positive x-axis
			 pDC->SetPixel(xpos, ytpos, clrArccosineColor);
			// Negative x-axis
			 pDC->SetPixel(xpos, ytposneg, clrArccosineColor);

		}
		
	}
}

void DrawArctan ( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrArctanColor)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	double j = 0.0;
	int n = 0;
	double value = 0.0;
	int nEndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = -nHalfHeight -0.7 * denominator ;
	CString strTemp;	
	
	int nCount = nEndY / (PI*denominator) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);
	
	for ( n = -nCount; n <= nCount; n++ )
	{
		for ( i = -nEndX; i <= nEndX; i += deltai )
		{
				value = atan(i * PI/180) + n *  PI;
				
			
			// 0.1 mm 
			// 100 * 0,1 mm = 10 mm = 1 cm 
			int ypos = ORIGIN.y + value * denominator;
			int xpos = ORIGIN.x +  i *PI/180 * denominator  ;
			pDC->SetPixel(xpos, ypos, clrArctanColor);
			
		}
		
	}
}

double cotan ( double x)
{
	return 1/tan(x);
}

double arccotan (double y)
{
	return atan(1/y);
}

void DrawArccotan ( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrArccotanColor)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	double j = 0.0;
	int n = 0;
	double value = 0.0;
	int nEndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = -nHalfHeight -0.7 * denominator ;
	CString strTemp;	
	
	int nCount = nEndY / (PI*denominator) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);
	
	for ( n = -nCount; n <= nCount; n++ )
	{
		for ( i = -nEndX; i <= nEndX; i += deltai )
		{
				value = arccotan(i * PI/180) + n *  PI;
				
			
			// 0.1 mm 
			// 100 * 0,1 mm = 10 mm = 1 cm 
			int ypos = ORIGIN.y + value * denominator;
			int xpos = ORIGIN.x +  i *PI/180 * denominator  ;
			pDC->SetPixel(xpos, ypos, clrArccotanColor);
			
		}
		
	}
}

void DrawHyperbolicSine ( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrHyperbolicSine)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	double j = 0.0;
	int n = 0;
	double value = 0.0;
	int nEndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = -nHalfHeight -0.7 * denominator ;
	CString strTemp;	
	
	int nCount = nEndY / (PI*denominator) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);

	for ( i = -nEndX; i <= nEndX ; i += deltai)
	{
		value = sinh( i * PI / 180);

		int xpos = ORIGIN.x + i*PI / 180 * denominator;
		int ypos = ORIGIN.y + value * denominator;
		
		pDC->SetPixel( xpos, ypos, clrHyperbolicSine);
	}
}

void DrawHyperbolicCosine( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrHyperbolicCosine)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	double j = 0.0;
	int n = 0;
	double value = 0.0;
	int nEndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = -nHalfHeight -0.7 * denominator ;
	CString strTemp;	
	
	int nCount = nEndY / (PI*denominator) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);

	for ( i = -nEndX; i <= nEndX ; i += deltai)
	{
		value = cosh( i * PI / 180);

		int xpos = ORIGIN.x + i*PI / 180 * denominator;
		int ypos = ORIGIN.y + value * denominator;
		
		pDC->SetPixel( xpos, ypos, clrHyperbolicCosine);
	}
}

void DrawHyperbolicTan( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrHyperbolicTan)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	double j = 0.0;
	int n = 0;
	double value = 0.0;
	int nEndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = -nHalfHeight -0.7 * denominator ;
	CString strTemp;	
	
	int nCount = nEndY / (PI*denominator) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);

	for ( i = -nEndX; i <= nEndX ; i += deltai)
	{
		value = tanh( i * PI / 180);

		int xpos = ORIGIN.x + i*PI / 180 * denominator;
		int ypos = ORIGIN.y + value * denominator;
		
		pDC->SetPixel( xpos, ypos, clrHyperbolicTan);
	}
}

double coth ( double x){
	return 1/tanh(x);
}

void DrawHyperbolicCotan( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect, COLORREF clrHyperbolicCotan)
{
	// Constants
	static const double PI = 3.1415926535897932384626433832795;
	static const double TWOPI = 2*PI;
	// Find the client rectangle
	CRect rcClient;
	HDC hDC=*pDC;
	HWND hWndFromDC =WindowFromDC(hDC);
	GetClientRect(hWndFromDC, &rcClient);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcClient.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcClient.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcClient.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcClient.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcClient = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcClient.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcClient.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcClient);

	int nHalfWidth = rcClient.Width() / 2;
	int nHalfHeight = rcClient.Height() / 2;	

	// CPoint ORIGIN(0, 0);
	CPoint ORIGIN(nHalfWidth, nHalfHeight);

	double i = 0.0;
	double deltai = 0.01;
	const int denominator = nTicksInterval;
	double j = 0.0;
	int n = 0;
	double value = 0.0;
	int nEndX = 180* ( rcClient.Width() / 2 ) / (PI * denominator) -0.05 * denominator;

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = -nHalfHeight -0.7 * denominator ;
	CString strTemp;	
	
	int nCount = nEndY / (PI*denominator) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);

	for ( i = -nEndX; i <= nEndX ; i += deltai)
	{
		value = coth( i * PI / 180);

		int xpos = ORIGIN.x + i*PI / 180 * denominator;
		int ypos = ORIGIN.y + value * denominator;
		
		pDC->SetPixel( xpos, ypos, clrHyperbolicCotan);
	}
}