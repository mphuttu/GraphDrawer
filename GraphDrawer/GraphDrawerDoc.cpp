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
#include "ExpressionParser.h"

#include <propkey.h>
#include <cmath>
#include <vector>
#include <utility>

namespace
{
	inline int ToInt(double value)
	{
		return static_cast<int>(value);
	}
}

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

	// Custom expression
	m_bDrawCustomFunction  = FALSE;
	m_strCustomExpression  = _T("");
	m_dCustomRangeStart    = -20.0;
	m_dCustomRangeEnd      =  20.0;
	m_bCancelDraw          = FALSE;
	m_pDrawThread          = NULL;
}

CGraphDrawerDoc::~CGraphDrawerDoc()
{
	// Ask any running worker thread to stop and wait for it.
	m_bCancelDraw = TRUE;
	if (m_pDrawThread != NULL)
	{
		WaitForSingleObject(m_pDrawThread->m_hThread, 3000);
		m_pDrawThread = NULL;
	}
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


// ---------------------------------------------------------------------------
// Custom expression  y = f(x)  —  thread-based computation
// ---------------------------------------------------------------------------

// Worker thread: evaluates the expression over the x-range and caches the
// resulting (x, y) math-coordinate pairs in m_vecCustomPoints.
// When finished it posts WM_APP to the main window so the view redraws.
UINT CGraphDrawerDoc::DrawThreadProc(LPVOID pParam)
{
	ThreadParams* p = reinterpret_cast<ThreadParams*>(pParam);
	CGraphDrawerDoc* pDoc = p->pDoc;

	try
	{
		CExpressionParser parser;
		if (!parser.Parse(p->strExpr))
		{
			delete p;
			return 1;
		}

		std::vector<MathPoint> pts;
		pts.reserve(static_cast<size_t>((p->xEnd - p->xStart) / p->xStep) + 1);

		for (double x = p->xStart; x <= p->xEnd; x += p->xStep)
		{
			if (pDoc->m_bCancelDraw)
				break;

			double y = 0.0;
			if (parser.Evaluate(x, y))
				pts.push_back({ x, y });
		}

		if (!pDoc->m_bCancelDraw)
		{
			CSingleLock lock(&pDoc->m_csCustomPoints, TRUE);
			pDoc->m_vecCustomPoints = std::move(pts);
		}
	}
	catch (const std::exception& ex)
	{
		// Log to debug output; do not crash the application.
		CString msg;
		msg.Format(_T("DrawThreadProc exception: %S"), ex.what());
		TRACE(msg);
	}
	catch (...)
	{
		TRACE(_T("DrawThreadProc: unknown exception caught.\n"));
	}

	delete p;

	// Ask the main window to trigger a repaint now that the data is ready.
	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd && !pDoc->m_bCancelDraw)
		pMainWnd->PostMessage(WM_APP, 0, 0);

	return 0;
}

// Called by the dialog whenever the expression or the enable-flag changes.
void CGraphDrawerDoc::SetCustomExpression(const CString& expr, BOOL bDraw,
	double xStart /*= -20.0*/, double xEnd /*= 20.0*/)
{
	m_bDrawCustomFunction = bDraw;
	m_strCustomExpression = expr;
	m_dCustomRangeStart   = xStart;
	m_dCustomRangeEnd     = xEnd;

	// Cancel any currently-running computation.
	m_bCancelDraw = TRUE;
	if (m_pDrawThread != NULL)
	{
		WaitForSingleObject(m_pDrawThread->m_hThread, 2000);
		m_pDrawThread = NULL;
	}
	m_bCancelDraw = FALSE;

	if (!bDraw || expr.IsEmpty())
	{
		CSingleLock lock(&m_csCustomPoints, TRUE);
		m_vecCustomPoints.clear();
		UpdateAllViews(NULL);
		return;
	}

	// Sanity-check the range so the worker thread can never get invalid params.
	if (!_finite(xStart) || !_finite(xEnd) || xStart >= xEnd)
	{
		AfxMessageBox(_T("Invalid x range: start must be a finite number less than end."),
		              MB_OK | MB_ICONWARNING);
		m_bDrawCustomFunction = FALSE;
		return;
	}

	// Quickly validate the expression before starting a thread.
	CExpressionParser test;
	if (!test.Parse(expr))
	{
		AfxMessageBox(_T("Expression error:\n") + test.GetError(), MB_OK | MB_ICONWARNING);
		m_bDrawCustomFunction = FALSE;
		return;
	}

	// Launch worker thread.  The range [-20, +20] with step 0.0005 gives
	// 80 000 sample points — enough detail for any reasonable expression
	// without being noticeably slow.
	ThreadParams* params  = new ThreadParams;
	params->pDoc          = this;
	params->strExpr       = expr;
	params->xStart        = xStart;
	params->xEnd          = xEnd;
	// Adapt step to always sample ~80 000 points regardless of range width.
	{
		double range = xEnd - xStart;
		double step  = range / 80000.0;
		if (step < 1e-10) step = 1e-10;
		params->xStep = step;
	}

	m_pDrawThread = AfxBeginThread(DrawThreadProc, params,
		THREAD_PRIORITY_BELOW_NORMAL, 0, CREATE_SUSPENDED);
	if (m_pDrawThread)
	{
		m_pDrawThread->m_bAutoDelete = TRUE;
		m_pDrawThread->ResumeThread();
	}
	else
	{
		delete params;
	}
}

// Draw the pre-computed custom-function points onto the DC.
// Called from CGraphDrawerView::OnDraw() with the same coordinate system
// used by all the other Draw* functions.
void DrawCustomFunction(CDC* pDC, int nTicksInterval, CRect /*m_rcPrintRect*/,
	COLORREF clrColor,
	CGraphDrawerDoc* pDoc)
{
	if (!pDoc->m_bDrawCustomFunction)
		return;

	CRect rcClient;
	HDC hDC = *pDC;
	HWND hWnd = WindowFromDC(hDC);
	GetClientRect(hWnd, &rcClient);
	pDC->DPtoLP(&rcClient);

	const int nHalfWidth  = rcClient.Width()  / 2;
	const int nHalfHeight = rcClient.Height() / 2;
	const CPoint ORIGIN(nHalfWidth, nHalfHeight);
	const int denom = nTicksInterval;

	CSingleLock lock(&pDoc->m_csCustomPoints, TRUE);
	const auto& pts = pDoc->m_vecCustomPoints;
	if (pts.empty())
		return;

	// Draw as a polyline: collect connected segments, breaking on gaps.
	bool firstPoint = true;
	CPoint prev;

	for (const auto& mp : pts)
	{
		int px = static_cast<int>(ORIGIN.x + mp.x * denom);
		int py = static_cast<int>(ORIGIN.y + mp.y * denom);

		// Skip points outside a generous clipping region.
		if (px < -32000 || px > 32000 || py < -32000 || py > 32000)
		{
			firstPoint = true;
			continue;
		}

		CPoint cur(px, py);
		if (firstPoint)
		{
			pDC->MoveTo(cur);
			firstPoint = false;
		}
		else
		{
			// If the y-jump is huge (discontinuity, e.g. tan asymptote), lift pen.
			if (abs(cur.y - prev.y) > abs(nHalfHeight) * 2)
				pDC->MoveTo(cur);
			else
				pDC->LineTo(cur);
		}
		prev = cur;
	}
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
	int EndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);
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
		int ypos = ToInt(ORIGIN.y + value * denominator);
		int xpos = ToInt(ORIGIN.x +  i*PI/180 * denominator  );
		int xposneg = ToInt(ORIGIN.x - i*PI/180 * denominator);
		int yposneg = ToInt(ORIGIN.y - value * denominator);
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
	int EndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = cos(  i * PI/180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ToInt(ORIGIN.y + value * denominator);
		int xpos = ToInt(ORIGIN.x +  i*PI/180 * denominator  );
		int xposneg = ToInt(ORIGIN.x - i*PI/180 * denominator);
		int yposneg = ToInt(ORIGIN.y + value * denominator);
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
	int EndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = tan(  i * PI/180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ToInt(ORIGIN.y + value * denominator);
		int xpos = ToInt(ORIGIN.x +  i*PI/180 * denominator  );
		int xposneg = ToInt(ORIGIN.x - i*PI/180 * denominator);
		int yposneg = ToInt(ORIGIN.y - value * denominator);
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
	int EndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = 1 / tan(  i * PI/180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ToInt(ORIGIN.y + value * denominator);
		int xpos = ToInt(ORIGIN.x +  i*PI/180 * denominator  );
		int xposneg = ToInt(ORIGIN.x - i*PI/180 * denominator);
		int yposneg = ToInt(ORIGIN.y - value * denominator);
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
	int EndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = exp(i * PI / 180);
		double valueneg = exp(-i * PI / 180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ToInt(ORIGIN.y + value * denominator);
		int xpos = ToInt(ORIGIN.x +  i *PI/180 * denominator  );
		int xposneg = ToInt(ORIGIN.x - i * PI/180 * denominator);
		int yposneg = ToInt(ORIGIN.y + valueneg * denominator);
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
	int EndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	for ( i = 0; i <= EndX; i += deltai )
	{
		double value = log(i * PI / 180);
		// double valueneg = exp(-i * PI / 180);
		// value += 1.0;

		// 0.1 mm 
		// 100 * 0,1 mm = 10 mm = 1 cm 
		int ypos = ToInt(ORIGIN.y + value * denominator);
		int xpos = ToInt(ORIGIN.x +  i *PI/180 * denominator  );
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
	int nEndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = ToInt(-nHalfHeight - PI*denominator );
	CString strTemp;
	
	
	
	int nCount = ToInt(nEndY / (PI*denominator)) ;
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
			int ypos = ToInt(ORIGIN.y + value * denominator);
			int ytpos = ToInt(ORIGIN.y + valuet * denominator);
			int xpos = ToInt(ORIGIN.x +  i *PI/180 * denominator  );
			int xposneg = ToInt(ORIGIN.x - i * PI/180 * denominator);
			int yposneg = ToInt(ORIGIN.y + valueneg * denominator);
			int ytposneg = ToInt(ORIGIN.y + valuenegt * denominator);
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
	int nEndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = ToInt(-nHalfHeight -0.7 * denominator);
	CString strTemp;
	
	
	
	int nCount = ToInt(nEndY / (PI * denominator)) ;
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
			int ypos = ToInt(ORIGIN.y + value * denominator);
			int ytpos = ToInt(ORIGIN.y + valuet * denominator);
			int xpos = ToInt(ORIGIN.x +  i *PI/180 * denominator  );
			int xposneg = ToInt(ORIGIN.x - i * PI/180 * denominator);
			int yposneg = ToInt(ORIGIN.y + valueneg * denominator);
			int ytposneg = ToInt(ORIGIN.y + valuenegt * denominator);
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
	int n = 0;
	double value = 0.0;
	int nEndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = ToInt(-nHalfHeight - 0.7 * denominator);
	CString strTemp;	
	
	int nCount = ToInt(nEndY / (PI * denominator)) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);
	
	for ( n = -nCount; n <= nCount; n++ )
	{
		for ( i = -nEndX; i <= nEndX; i += deltai )
		{
				value = atan(i * PI/180) + n *  PI;
				
			
			// 0.1 mm 
			// 100 * 0,1 mm = 10 mm = 1 cm 
			int ypos = ToInt(ORIGIN.y + value * denominator);
			int xpos = ToInt(ORIGIN.x +  i *PI/180 * denominator  );
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
	int nEndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = ToInt(-nHalfHeight - 0.7 * denominator);
	CString strTemp;	
	
	int nCount = ToInt(nEndY / (PI * denominator)) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);
	
	for ( n = -nCount; n <= nCount; n++ )
	{
		for ( i = -nEndX; i <= nEndX; i += deltai )
		{
				value = arccotan(i * PI/180) + n *  PI;
				
			
			// 0.1 mm 
			// 100 * 0,1 mm = 10 mm = 1 cm 
			int ypos = ToInt(ORIGIN.y + value * denominator);
			int xpos = ToInt(ORIGIN.x +  i *PI/180 * denominator  );
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
	double value = 0.0;
	int nEndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	for ( i = -nEndX; i <= nEndX ; i += deltai)
	{
		value = sinh( i * PI / 180);

		int xpos = ToInt(ORIGIN.x + i*PI / 180 * denominator);
		int ypos = ToInt(ORIGIN.y + value * denominator);
		
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
	double value = 0.0;
	int nEndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	for ( i = -nEndX; i <= nEndX ; i += deltai)
	{
		value = cosh( i * PI / 180);

		int xpos = ToInt(ORIGIN.x + i*PI / 180 * denominator);
		int ypos = ToInt(ORIGIN.y + value * denominator);
		
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
	double value = 0.0;
	int nEndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	for ( i = -nEndX; i <= nEndX ; i += deltai)
	{
		value = tanh( i * PI / 180);

		int xpos = ToInt(ORIGIN.x + i*PI / 180 * denominator);
		int ypos = ToInt(ORIGIN.y + value * denominator);
		
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
	int nEndX = ToInt(180 * (rcClient.Width() / 2) / (PI * denominator) - 0.05 * denominator);

	// int nEndY = -180 * (rcClient.Height() / 2 ) / (PI * denominator ) - 0.3 * denominator;
	int nEndY = ToInt(-nHalfHeight - 0.7 * denominator);
	CString strTemp;	
	
	int nCount = ToInt(nEndY / (PI * denominator)) ;
	strTemp.Format(_T("nEndY is %d and nCount is %d"), nEndY, nCount);
	// AfxMessageBox(strTemp);
	
	for ( i = -nEndX; i <= nEndX ; i += deltai)
	{
		value = coth( i * PI / 180);

		int xpos = ToInt(ORIGIN.x + i*PI / 180 * denominator);
		int ypos = ToInt(ORIGIN.y + value * denominator);
		
		pDC->SetPixel( xpos, ypos, clrHyperbolicCotan);
	}
}