
// GraphDrawerView.cpp : implementation of the CGraphDrawerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GraphDrawer.h"
#endif

#include "GraphDrawerDoc.h"
#include "GraphDrawerView.h"
//#include "DrawOptionsDialog.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGraphDrawerView

IMPLEMENT_DYNCREATE(CGraphDrawerView, CScrollView)

BEGIN_MESSAGE_MAP(CGraphDrawerView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_APP, &CGraphDrawerView::OnCustomFunctionReady)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()
// Skaalauksen säilyttävä OnSize
void CGraphDrawerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	// Älä muuta skaalausta, muuta vain näkyvää aluetta
	if (cx <= 0 || cy <= 0) return;
	// Laske vanha leveys ja korkeus pikseleinä
	static int prevW = 0, prevH = 0;
	if (prevW == 0 || prevH == 0) { prevW = cx; prevH = cy; return; }
	double xRange = m_dViewXMax - m_dViewXMin;
	double yRange = m_dViewYMax - m_dViewYMin;
	double scaleX = xRange / prevW;
	double scaleY = yRange / prevH;
	double cxMid = (m_dViewXMin + m_dViewXMax) * 0.5;
	double cyMid = (m_dViewYMin + m_dViewYMax) * 0.5;
	double newXRange = scaleX * cx;
	double newYRange = scaleY * cy;
	m_dViewXMin = cxMid - newXRange * 0.5;
	m_dViewXMax = cxMid + newXRange * 0.5;
	m_dViewYMin = cyMid - newYRange * 0.5;
	m_dViewYMax = cyMid + newYRange * 0.5;
	prevW = cx; prevH = cy;
	Invalidate(FALSE);
}

// CGraphDrawerView construction/destruction

CGraphDrawerView::CGraphDrawerView()
	: m_dViewXMin(-10.0), m_dViewXMax(10.0)
	, m_dViewYMin(-10.0), m_dViewYMax(10.0)
	, m_bDragging(FALSE)
{
}

CGraphDrawerView::~CGraphDrawerView()
{
}

BOOL CGraphDrawerView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// ---------------------------------------------------------------------------
// Range / transform helpers
// ---------------------------------------------------------------------------

void CGraphDrawerView::SyncRangeFromDoc()
{
	CGraphDrawerDoc* pDoc = GetDocument();
	if (!pDoc) return;
	m_dViewXMin = pDoc->m_DrawOptionsData.dXMin;
	m_dViewXMax = pDoc->m_DrawOptionsData.dXMax;
	m_dViewYMin = pDoc->m_DrawOptionsData.dYMin;
	m_dViewYMax = pDoc->m_DrawOptionsData.dYMax;
}

CoordTransform CGraphDrawerView::BuildCoordTransform(CDC* pDC) const
{
	const DrawOptionsData& opts = GetDocument()->m_DrawOptionsData;

	// -----------------------------------------------------------------------
	// TULOSTUS: MM_LOMETRIC-koordinaatit (bYDown=false, Y kasvaa ylöspäin)
	// -----------------------------------------------------------------------
	if (pDC->IsPrinting())
	{
		CRect rc = m_rcPrintRect;
		pDC->DPtoLP(&rc);
		int w =  rc.Width();
		int h = -rc.Height();
		if (w <= 0) w = 1;
		if (h <= 0) h = 1;

		double xMin = m_dViewXMin, xMax = m_dViewXMax;
		double yMin = m_dViewYMin, yMax = m_dViewYMax;

		if (opts.scaleMode == 0) {
			double xRange = xMax - xMin, yRange = yMax - yMin;
			double scale = max(xRange / w, yRange / h);
			double cx = (xMin + xMax) * 0.5, cy = (yMin + yMax) * 0.5;
			xMin = cx - scale * w * 0.5; xMax = cx + scale * w * 0.5;
			yMin = cy - scale * h * 0.5; yMax = cy + scale * h * 0.5;
		} else if (opts.scaleMode == 1) {
			double xRange = w / (opts.scaleX * 100.0);
			double yRange = h / (opts.scaleY * 100.0);
			double cx = (m_dViewXMin + m_dViewXMax) * 0.5;
			double cy = (m_dViewYMin + m_dViewYMax) * 0.5;
			xMin = cx - xRange * 0.5; xMax = cx + xRange * 0.5;
			yMin = cy - yRange * 0.5; yMax = cy + yRange * 0.5;
		} else if (opts.scaleMode == 2) {
			bool bLogX = (opts.logMode == 0 || opts.logMode == 2);
			bool bLogY = (opts.logMode == 1 || opts.logMode == 2);
			if (bLogX) { if (xMin <= 0.0) xMin = 1e-3; if (xMax <= xMin) xMax = xMin * 1000.0; }
			if (bLogY) { if (yMin <= 0.0) yMin = 1e-3; if (yMax <= yMin) yMax = yMin * 1000.0; }
			return CoordTransform(xMin, xMax, yMin, yMax, w, h, bLogX, bLogY, false);
		}
		return CoordTransform(xMin, xMax, yMin, yMax, w, h, false, false, false);
	}

	// -----------------------------------------------------------------------
	// NÄYTTÖ: pikselikoordinaatit (bYDown=true, Y kasvaa alaspäin kuten MM_TEXT).
	// GetClientRect palauttaa pikselit suoraan — ei tarvita DPtoLP-muunnosta.
	// -----------------------------------------------------------------------
	CRect rc;
	const_cast<CGraphDrawerView*>(this)->GetClientRect(&rc);
	int w = rc.Width();
	int h = rc.Height();
	if (w <= 0) w = 1;
	if (h <= 0) h = 1;

	double xMin = m_dViewXMin, xMax = m_dViewXMax;
	double yMin = m_dViewYMin, yMax = m_dViewYMax;

	if (opts.scaleMode == 0) {
		// -------------------------------------------------------------------
		// Tasainen skaalaus: kompensoidaan pikseliaspektisuhde niin, että
		// 1 matemaattinen yksikkö vie yhtä monta pikseliä x- ja y-suunnassa.
		//   aspect = w/h  →  laajennetaan pienempi akseli.
		// -------------------------------------------------------------------
		double xRange = xMax - xMin;
		double yRange = yMax - yMin;
		double aspect = (double)w / (double)h;
		if (aspect >= 1.0) {
			double newXRange = yRange * aspect;
			double cx = (xMin + xMax) * 0.5;
			xMin = cx - newXRange * 0.5;
			xMax = cx + newXRange * 0.5;
		} else {
			double newYRange = xRange / aspect;
			double cy = (yMin + yMax) * 0.5;
			yMin = cy - newYRange * 0.5;
			yMax = cy + newYRange * 0.5;
		}
	} else if (opts.scaleMode == 1) {
		// -------------------------------------------------------------------
		// Vapaa skaalaus: scaleX/scaleY = cm per matemaattinen yksikkö.
		// Muunnetaan pikseleihin käyttäen näytön oikeita DPI-arvoja.
		//   1 inch = 2.54 cm  →  pxPerUnit = scaleX * dpi / 2.54
		// -------------------------------------------------------------------
		int dpiX = pDC->GetDeviceCaps(LOGPIXELSX);
		int dpiY = pDC->GetDeviceCaps(LOGPIXELSY);
		if (dpiX <= 0) dpiX = 96;
		if (dpiY <= 0) dpiY = 96;
		double pxPerUnitX = opts.scaleX * dpiX / 2.54;
		double pxPerUnitY = opts.scaleY * dpiY / 2.54;
		double xRange = w / pxPerUnitX;
		double yRange = h / pxPerUnitY;
		double cx = (m_dViewXMin + m_dViewXMax) * 0.5;
		double cy = (m_dViewYMin + m_dViewYMax) * 0.5;
		xMin = cx - xRange * 0.5; xMax = cx + xRange * 0.5;
		yMin = cy - yRange * 0.5; yMax = cy + yRange * 0.5;
	} else if (opts.scaleMode == 2) {
		// Logaritminen asteikko: logMode määrää kumpi akseli on log
		bool bLogX = (opts.logMode == 0 || opts.logMode == 2);
		bool bLogY = (opts.logMode == 1 || opts.logMode == 2);
		if (bLogX) { if (xMin <= 0.0) xMin = 1e-3; if (xMax <= xMin) xMax = xMin * 1000.0; }
		if (bLogY) { if (yMin <= 0.0) yMin = 1e-3; if (yMax <= yMin) yMax = yMin * 1000.0; }
		return CoordTransform(xMin, xMax, yMin, yMax, w, h, bLogX, bLogY, true);
	}
	return CoordTransform(xMin, xMax, yMin, yMax, w, h, false, false, true);
}

// ---------------------------------------------------------------------------
// Zoom and pan
// ---------------------------------------------------------------------------

BOOL CGraphDrawerView::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint pt)
{
	// Zoom around the current viewport center.
	double factor = (zDelta > 0) ? 0.8 : 1.25;

	double cx = (m_dViewXMin + m_dViewXMax) * 0.5;
	double cy = (m_dViewYMin + m_dViewYMax) * 0.5;
	double hw = (m_dViewXMax - m_dViewXMin) * 0.5 * factor;
	double hh = (m_dViewYMax - m_dViewYMin) * 0.5 * factor;

	// Clamp to prevent extreme zoom.
	if (hw < 1e-4 || hh < 1e-4) return TRUE;
	if (hw > 1e6  || hh > 1e6)  return TRUE;

	m_dViewXMin = cx - hw;  m_dViewXMax = cx + hw;
	m_dViewYMin = cy - hh;  m_dViewYMax = cy + hh;

	Invalidate(TRUE);
	return TRUE;
}

void CGraphDrawerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDragging  = TRUE;
	m_ptLastMouse = point;
	SetCapture();
	CScrollView::OnLButtonDown(nFlags, point);
}

void CGraphDrawerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDragging = FALSE;
	ReleaseCapture();
	CScrollView::OnLButtonUp(nFlags, point);
}

void CGraphDrawerView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		int w = rcClient.Width();
		int h = rcClient.Height();
		if (w <= 0 || h <= 0) return;

		const DrawOptionsData& opts = GetDocument()->m_DrawOptionsData;
		double dx_math, dy_math;

		if (opts.scaleMode == 0) {
			// Tasainen skaalaus: molemmat akselit samalla pikseliä/yksikkö suhteella.
			// "Vetävä" akseli on se, jonka range ei laajennettu aspektikorjauksessa.
			double yRange = m_dViewYMax - m_dViewYMin;
			double xRange = m_dViewXMax - m_dViewXMin;
			double mpx = ((double)w >= (double)h)
				? yRange / h   // landscape: y on veturi
				: xRange / w;  // portrait:  x on veturi
			dx_math =  (double)(point.x - m_ptLastMouse.x) * mpx;
			dy_math = -(double)(point.y - m_ptLastMouse.y) * mpx;  // Y-akseli ylösalaisin
		} else if (opts.scaleMode == 1) {
			// Vapaa skaalaus: käytetään DPI-pohjaista muunnosta
			CClientDC dc(this);
			int dpiX = dc.GetDeviceCaps(LOGPIXELSX);
			int dpiY = dc.GetDeviceCaps(LOGPIXELSY);
			if (dpiX <= 0) dpiX = 96;
			if (dpiY <= 0) dpiY = 96;
			double pxPerUnitX = opts.scaleX * dpiX / 2.54;
			double pxPerUnitY = opts.scaleY * dpiY / 2.54;
			dx_math =  (double)(point.x - m_ptLastMouse.x) / pxPerUnitX;
			dy_math = -(double)(point.y - m_ptLastMouse.y) / pxPerUnitY;
		} else {
			// Muut tilat: yksinkertainen pikselisuhde
			double xRange = m_dViewXMax - m_dViewXMin;
			double yRange = m_dViewYMax - m_dViewYMin;
			dx_math =  (double)(point.x - m_ptLastMouse.x) * xRange / w;
			dy_math = -(double)(point.y - m_ptLastMouse.y) * yRange / h;
		}

		m_dViewXMin -= dx_math;  m_dViewXMax -= dx_math;
		m_dViewYMin -= dy_math;  m_dViewYMax -= dy_math;

		m_ptLastMouse = point;
		Invalidate(TRUE);
	}
	CScrollView::OnMouseMove(nFlags, point);
}

// Right-click resets the view range to doc defaults.
void CGraphDrawerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SyncRangeFromDoc();
	Invalidate(TRUE);
	CScrollView::OnRButtonDown(nFlags, point);
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

void CGraphDrawerView::OnDraw(CDC* pDC)
{
	CGraphDrawerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Set mapping mode: MM_TEXT (pixels) for screen, MM_LOMETRIC for printing.
	// Printing already has MM_LOMETRIC set by OnPrint before OnDraw is called.
	if (pDC->IsPrinting())
		pDC->SetMapMode(MM_LOMETRIC);
	else
		pDC->SetMapMode(MM_TEXT);

	// Build the coordinate transform.
	// For screen: uses GetClientRect; for print: uses m_rcPrintRect.
	CoordTransform ct = BuildCoordTransform(pDC);

	// Save the current state of device context
	int nDC = pDC->SaveDC();
	
	// Create font for axis labels
	CFont AxisFont;
	if ( AxisFont.CreateFont(24, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, FF_ROMAN, _T("Arial") ) )
		pDC->SelectObject(&AxisFont);
	else
	{
		AfxMessageBox(L"Unable to create Axis font");
		return;
	}

	CPen AxisPen;
	if ( AxisPen.CreatePen( PS_SOLID, pDoc->m_nAxesPenWidth, pDoc->m_clrAxesColor ) )
		pDC->SelectObject(&AxisPen);
	else
	{
		AfxMessageBox(L"Unable to create Axis Pen");
		return;
	}

	// Draw Coordinates
	if ( pDoc->m_bShowCoordinateAxes == TRUE )
		DrawCoordinateAxes(pDC, pDoc->m_bShowTicks, 
		pDoc->m_nTicksInterval, pDoc->m_bShowTicksLabel, ct);

	// Built-in function curves
	if ( pDoc->m_bDrawSine == TRUE)
		DrawSine(pDC, ct, RGB(255, 51, 51));

	if ( pDoc->m_bDrawCosine == TRUE)
		DrawCosine(pDC, ct, RGB(51, 255, 51));
	
	if ( pDoc->m_bDrawTan == TRUE)
		DrawTan(pDC, ct, RGB(51, 153, 255));

	if ( pDoc->m_bDrawCotan == TRUE )
		DrawCotan(pDC, ct, RGB(255, 58, 0));

	if ( pDoc->m_bDrawExp == TRUE )
		DrawExp(pDC, ct, RGB(153, 0, 0));

	if ( pDoc->m_bDrawLN == TRUE )
		DrawLN(pDC, ct, RGB(0, 102, 102));

	if ( pDoc->m_bDrawArcsine == TRUE )
		DrawArcsine(pDC, ct, RGB(204, 0, 204));

	if ( pDoc->m_bDrawArccosine == TRUE)
		DrawArccosine(pDC, ct, RGB(167, 198, 104));

	if ( pDoc->m_bDrawArctan == TRUE )
		DrawArctan(pDC, ct, RGB(255, 128, 0));

	if ( pDoc->m_bDrawArccotan == TRUE )
		DrawArccotan(pDC, ct, RGB(245, 217, 41));

	if ( pDoc->m_bDrawHyperbolicSine == TRUE )
		DrawHyperbolicSine(pDC, ct, RGB(255, 51, 51));

	if ( pDoc->m_bDrawHyperbolicCosine == TRUE )
		DrawHyperbolicCosine(pDC, ct, RGB(0, 102, 0));

	if ( pDoc->m_bDrawHyperbolicTan == TRUE )
		DrawHyperbolicTan(pDC, ct, RGB(90, 146, 174));

	if ( pDoc->m_bDrawHyperbolicCotan == TRUE )
		DrawHyperbolicCotan(pDC, ct, RGB(51, 0, 102));

	// Draw custom expression  y = f(x)
	if ( pDoc->m_bDrawCustomFunction == TRUE )
	{
		CPen customPen;
		CPen* pOldCustomPen = nullptr;
		if ( customPen.CreatePen(PS_SOLID, 1, RGB(255, 200, 0)) )
			pOldCustomPen = pDC->SelectObject(&customPen);
		DrawCustomFunction(pDC, ct, RGB(255, 200, 0), pDoc);
		// Deselect before customPen goes out of scope; otherwise DeleteObject fails
		// and the GDI handle leaks, eventually exhausting the GDI handle pool.
		if (pOldCustomPen)
			pDC->SelectObject(pOldCustomPen);
	}

	// Draw user-defined curves
	DrawUserCurves(pDC, ct, pDoc->m_vecUserCurves, pDoc->m_csUserCurves);

	// Restore Original device context
	pDC->RestoreDC(nDC);
}

void CGraphDrawerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// Set the scroll sizes
	CGraphDrawerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	SetScrollSizes(MM_LOMETRIC, pDoc->GetDocSize() );

	// Initialize view range from document settings.
	SyncRangeFromDoc();
}

// CGraphDrawerView printing

BOOL CGraphDrawerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(1);  // one-page output
	return DoPreparePrinting(pInfo);
}

void CGraphDrawerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
	// Create special font for printing
	// ... prepare LOGFONT structure
	LOGFONT lf; // logical font structure
	::ZeroMemory( &lf, sizeof(lf) );

	// 12 pt in MM_LOMETRIC logical units (0.1 mm each): 12 * 25.4 mm / 72 pt * 10 = 42.3
	lf.lfHeight = -MulDiv(12, 254, 72);  // ≈ -42 units = 12pt in MM_LOMETRIC
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_BOLD;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Arial");

	// ... Create printing font
	m_PrintFont.CreateFontIndirectW(&lf);
}

void CGraphDrawerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
	m_PrintFont.DeleteObject();
}


// CGraphDrawerView diagnostics

#ifdef _DEBUG
void CGraphDrawerView::AssertValid() const
{
	CView::AssertValid();
}

void CGraphDrawerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGraphDrawerDoc* CGraphDrawerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphDrawerDoc)));
	return (CGraphDrawerDoc*)m_pDocument;
}
#endif //_DEBUG


// CGraphDrawerView message handlers


void CGraphDrawerView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// Set mapping mode before header/footer so DPtoLP works correctly.
	pDC->SetMapMode(MM_LOMETRIC);

	if (pInfo != NULL)
		m_rcPrintRect = pInfo->m_rectDraw;

	PrintPageHeader(pDC, pInfo, m_rcPrintRect);
	PrintPageFooter(pDC, pInfo, m_rcPrintRect);

	// CView::OnPrint calls OnDraw which renders the full graph.
	CView::OnPrint(pDC, pInfo);
}


void CGraphDrawerView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	
	
	// pDC->SetTextAlign(TA_LEFT);
	// Set the mapping mode
	// pDC->SetMapMode(MM_LOMETRIC); 
	 // Declare a client rectangle
	// CRect rcClient;
	// Not printing, so client rect will do
	// GetClientRect(&rcClient);
	// CSize sz(rcClient.Width(), rcClient.Height()); 
          
		 // pDC->SetMapMode(MM_LOENGLISH);
   //      pDC->SetViewportOrg( sz.cx/2, sz.cy/2 ); 
	// pDC->SetViewportOrg(0, 0 ); 
		 

		 ASSERT_VALID(pDC);
		 UNUSED(pDC);

		 // Default to one page printing if doc length is not known
		 if ( pInfo != NULL )
			 pInfo->m_bContinuePrinting = ( pInfo->GetMaxPage() != 0xffff ||
			 ( pInfo->m_nCurPage == 1 ) );

		 /*
		 // Continue printing until the query finishes
		 CGraphDrawerDoc* pDoc = GetDocument();
		 ASSERT_VALID(pDoc);

		 if ( pInfo != 0 )
			 pInfo->m_bContinuePrinting = !pDoc->QueryFinished();
			 */

		 /*
		 if(pDC->IsPrinting()) 
		 { 
			pDC->SetMapMode(MM_LOMETRIC); 
			pDC->SetWindowExt(GetDocument()->GetDocSize()); 
			pDC->SetViewportExt(2300,3200);
		 }
		 */

		 /*
		 if ( pDC->IsPrinting() )
		 {
			 int pageHeight = pDC->GetDeviceCaps(VERTRES);
			 int OriginY = pageHeight * ( pInfo->m_nCurPage -1 );
			 pDC->SetViewportOrg(0, -OriginY );
		 }

		 */
	CView::OnPrepareDC(pDC, pInfo);
}


void CGraphDrawerView::PrintPageHeader(CDC* pDC, CPrintInfo* pInfo, CRect rcPrint)
{
	// Map mode (MM_LOMETRIC) must be active before this call.
	CRect rcPage = rcPrint;
	pDC->DPtoLP(&rcPage);

	CString strHeader = _T("Graph: ");
	const CString path = GetDocument()->GetPathName();
	strHeader += path.IsEmpty() ? _T("(unsaved)") : path;

	CFont* pOldFont = pDC->SelectObject(&m_PrintFont);
	pDC->SetTextAlign(TA_LEFT);
	// In MM_LOMETRIC Y increases upward, so subtract margin to move DOWN from top.
	pDC->TextOutW(rcPage.left, rcPage.top - TOP_MARGIN, strHeader);
	pDC->SelectObject(pOldFont);
}


void CGraphDrawerView::PrintPageFooter(CDC* pDC, CPrintInfo* pInfo, CRect rcPrint)
{
	// Map mode (MM_LOMETRIC) must be active before this call.
	CRect rcPage = rcPrint;
	pDC->DPtoLP(&rcPage);

	CString strFooter;
	strFooter.Format(_T("Page %d of %d"), pInfo->m_nCurPage,
	                 pInfo->GetToPage() - pInfo->GetFromPage() + 1);

	CSize sizeFooter = pDC->GetTextExtent(strFooter);

	// In MM_LOMETRIC Y increases upward: bottom is the most-negative Y value.
	// Add BOTTOM_MARGIN to move UP (toward centre of page) from the bottom edge.
	int nLineY    = rcPage.bottom + BOTTOM_MARGIN;
	// Text sits above the line: add height (positive = upward in MM_LOMETRIC).
	int nTextY    = nLineY + sizeFooter.cy;

	pDC->MoveTo(rcPage.left,  nLineY);
	pDC->LineTo(rcPage.right, nLineY);

	CFont* pOldFont = pDC->SelectObject(&m_PrintFont);
	pDC->TextOutW(rcPage.left + (rcPage.Width() - sizeFooter.cx) / 2,
	              nTextY, strFooter);
	pDC->SelectObject(pOldFont);
}


void CGraphDrawerView::PrintPageBody(CDC* pDC, CPrintInfo* pInfo)
{
	CFont* pOldFont = pDC->SelectObject( &m_PrintFont);

	// 1 - Compute which elements we will print on this page

	// 8 - Clean Up
	pDC->SelectObject( pOldFont);
}


void CGraphDrawerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if ( pHint != NULL )
	{
		 if ( pHint->IsKindOf(RUNTIME_CLASS(CGraphDrawerDoc) ) )
		 {
			 // Update the scroll sizes
			 CGraphDrawerDoc* pDoc = GetDocument();
			 ASSERT_VALID(pDoc);
			 SetScrollSizes(MM_LOMETRIC, pDoc->GetDocSize() );
		 }
	}

	// If hint==1, a coordinate range change was triggered from Draw Options.
	if (lHint == 1)
		SyncRangeFromDoc();

	// Invalidate the entire view
	Invalidate();
}


BOOL CGraphDrawerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CGraphDrawerDoc * pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		 return FALSE;

	// Set the brush to desired background color
	CBrush backBrush (pDoc->m_clrBkgndColor);

	// Save old brush 
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);

	//CRect rect;
	// pDC->GetClipBox(&rect); // Erase the area needed

	// pDC->PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	CRect rcClient;
	GetClientRect(&rcClient);
	pDC->FillRect(rcClient, &backBrush);
	pDC->SelectObject(pOldBrush);

	return TRUE;
	// return CScrollView::OnEraseBkgnd(pDC);
}

// Called via PostMessage(WM_APP) from the worker thread when the custom
// function computation is complete.  Triggers a repaint.
LRESULT CGraphDrawerView::OnCustomFunctionReady(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	Invalidate(TRUE);
	return 0;
}
