
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
END_MESSAGE_MAP()

// CGraphDrawerView construction/destruction

CGraphDrawerView::CGraphDrawerView()
{
	// TODO: add construction code here

}

CGraphDrawerView::~CGraphDrawerView()
{
}

BOOL CGraphDrawerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CGraphDrawerView drawing

void CGraphDrawerView::OnDraw(CDC* pDC)
{
	/*
	CMainFrame * pWnd = dynamic_cast<CMainFrame *> (AfxGetMainWnd() );
	ASSERT_VALID(pWnd);
	*/

	CGraphDrawerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	
	// ** Set metric mapping
	pDC->SetMapMode(MM_LOMETRIC);
	// pDC->SetMapMode(MM_LOENGLISH);
	// Declare a client rectangle
	CRect rcClient;

	/*
	// ** Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// ** Printing, so use the print rectangle
		rcClient = m_rcPrintRect;
	}
	else 
	{
		// ** Not printing, so client rect will do
		GetClientRect(&rcClient);
	}

	*/

	
	// Not printing, so client rect will do
	GetClientRect(&rcClient);

	
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
	
	
	// ** Convert to logical units
	pDC->DPtoLP(&rcClient);
	

	// Save the current state of device context
	int nDC = pDC->SaveDC();
	
	// Create font for axis labels
	CFont AxisFont;

	if ( AxisFont.CreateFont(24, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, FF_ROMAN, _T("Arial") ) )
	{
		pDC->SelectObject(&AxisFont);
	}
	else
	{
		AfxMessageBox(L"Unable to create Axis font");
		return;
	}

	CPen AxisPen;

	if ( AxisPen.CreatePen( PS_SOLID, pDoc->m_nAxesPenWidth, pDoc->m_clrAxesColor ) )  // Orig: Axes Color RGB(0,0,0)
		pDC->SelectObject(&AxisPen);
	else
	{
		AfxMessageBox(L"Unable to create Axis Pen");
		return;
	}

	// Draw Coordinates
	if ( pDoc->m_bShowCoordinateAxes == TRUE )
		DrawCoordinateAxes(pDC, pDoc->m_bShowTicks, 
		pDoc->m_nTicksInterval, pDoc->m_bShowTicksLabel, m_rcPrintRect);	

	// Draw Sine

	if ( pDoc->m_bDrawSine == TRUE)
		DrawSine ( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(255, 51, 51));

	// Draw Cosine
	if ( pDoc->m_bDrawCosine == TRUE)
		DrawCosine ( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(51, 255, 51));
	
	// Draw Tan
	if ( pDoc->m_bDrawTan == TRUE)
		DrawTan ( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(51, 153, 255));

	// Draw Cotan
	if ( pDoc->m_bDrawCotan == TRUE )
		DrawCotan ( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB( 255, 58 , 0) );

	// Draw Exponent function
	if ( pDoc->m_bDrawExp == TRUE )
		DrawExp( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(153, 0, 0) );

	// Draw Natural Logarithm
	if ( pDoc->m_bDrawLN == TRUE )
		DrawLN( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(0, 102, 102) );

	// Draw Arcus Sine
	if ( pDoc->m_bDrawArcsine == TRUE )
		DrawArcsine( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(204, 0, 204) );

	// Draw Arcus Cosine
	if ( pDoc->m_bDrawArccosine == TRUE)
		DrawArccosine( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(167, 198,  104) );

	// Draw Arcus Tangent
	if ( pDoc->m_bDrawArctan == TRUE )
		DrawArctan ( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(255, 128, 0 ) );

	// Draw Arcus Cotangent
	if ( pDoc->m_bDrawArccotan == TRUE )
		DrawArccotan( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(245, 217, 41) );

	// Draw Hyperbolic Sine
	if ( pDoc->m_bDrawHyperbolicSine == TRUE )
		DrawHyperbolicSine( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB(255, 51 , 51) );

	// Draw Hyperbolic Cosine
	if ( pDoc->m_bDrawHyperbolicCosine == TRUE )
		DrawHyperbolicCosine( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB( 0, 102, 0 ) );

	// Draw Hyperbolic Tangent
	if ( pDoc->m_bDrawHyperbolicTan == TRUE )
		DrawHyperbolicTan ( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB( 90, 146, 174) );

	// Draw Hyperbolic Tangent
	if ( pDoc->m_bDrawHyperbolicCotan == TRUE )
		DrawHyperbolicCotan ( pDC, pDoc->m_nTicksInterval, m_rcPrintRect, RGB( 51, 0, 102) );

	// Restore Original device context
	pDC->RestoreDC(nDC);
}

void CGraphDrawerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	/*
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;

	// Calculate the total size of this view
	sizeTotal.cx = 850;
	sizeTotal.cy = 1100;

	SetScrollSizes( MM_LOMETRIC, sizeTotal);
	*/

	// Set the scroll sizes
	CGraphDrawerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	SetScrollSizes(MM_LOMETRIC, pDoc->GetDocSize() );
	// SetScrollSizes(MM_LOENGLISH, pDoc->GetDocSize() );
}

// CGraphDrawerView printing

BOOL CGraphDrawerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Get the number of pages in the print job
	// CGraphDrawerDoc* pDoc = GetDocument();
	// ASSERT_VALID(pDoc);
	// int nPages = pDoc->CalcNumPages();
	// pInfo->SetMaxPage(nPages);
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(3);

	/*
	pInfo->m_pPD->m_pd.nCopies = 3;

	do 
	{
		// ** Check if user has cancelled print
		if ( !DoPreparePrinting(pInfo) == FALSE )
			return FALSE;

		// ** Warn the user if too many copies are specified
		if ( pInfo->m_pPD->GetCopies() > 5 )
			AfxMessageBox(_T("Please choose less than 5 copies"));

		// ** Keep looping until they specify a valid number
	} while ( pInfo->m_pPD->GetCopies() > 5);

	return TRUE;
	*/
	return DoPreparePrinting(pInfo);
}

void CGraphDrawerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
	// Create special font for printing
	// ... prepare LOGFONT structure
	LOGFONT lf; // logical font structure
	::ZeroMemory( &lf, sizeof(lf) );

	// .. 12 point Times Bold
	lf.lfHeight = -MulDiv( 12, pDC->GetDeviceCaps(LOGPIXELSX), 72 );
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_BOLD;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;

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
	// TODO: Add your specialized code here and/or call the base class
	// Get a pointer to the document
	// CGraphDrawerDoc* pDoc = GetDocument();
	// ASSERT_VALID(pDoc);

	
	
	if (pInfo != 0) {
		 m_rcPrintRect= pInfo->m_rectDraw;
	}
	
	// pDC->SetWindowOrg( m_rcPrintRect.CenterPoint().x, m_rcPrintRect.top);
	// pDC->SetWindowOrg(  pInfo->m_rectDraw.left, pInfo->m_rectDraw.BottomRight().y);
	//pDC->SetWindowOrg( -pInfo->m_rectDraw.right,1000);
	// ** Print the page header and the footer and adjust the DC window origin
	PrintPageHeader( pDC, pInfo, m_rcPrintRect);
	PrintPageFooter (pDC, pInfo, m_rcPrintRect);	

	 // Print the document data
	// OnDraw(pDC);
	
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


void CGraphDrawerView::PrintPageHeader(CDC* pDC, CPrintInfo* pInfo, CRect m_rcPrintRect)
{
	// CRect& rcPage = pInfo->m_rectDraw;

	// Find the client rectangle
	CRect rcPage;
	GetClientRect(&rcPage);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcPage.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcPage.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcPage.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcPage.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcPage = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcPage.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcPage.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcPage);

	// 1 - Prepare page header
	CString strHeader = _T( "Graph: " );
	strHeader += GetDocument()->GetPathName();
	CSize sizeHeader = pDC->GetTextExtent( strHeader);

	// 2 - Draw header
	int nCurrentY = rcPage.top + TOP_MARGIN;
	// Draw the header text aligned left
	CFont* pOldFont = pDC->SelectObject(&m_PrintFont);
	pDC->SetTextAlign(TA_LEFT);
	pDC->TextOutW(0, nCurrentY, strHeader );
	// nCurrentY -= sizeHeader.cy*2;

	// 3 - Draw Line
	// pDC->MoveTo( 0, nCurrentY + 10);
	// pDC->LineTo( rcPage.right, nCurrentY + 10);

	// 4 - Adjust remaining printable area
	rcPage.top += nCurrentY;
	pDC->SelectObject(pOldFont);
}


void CGraphDrawerView::PrintPageFooter(CDC* pDC, CPrintInfo* pInfo, CRect m_rcPrintRect)
{
	// CRect& rcPage = pInfo->m_rectDraw;
	// Find the client rectangle
	CRect rcPage;
	GetClientRect(&rcPage);

	// Check the device context for printing mode
	if ( pDC->IsPrinting() == TRUE )
	{
		// Find the Print width: Window width ratio
		double dWidthRatio = (double) m_rcPrintRect.Width() 
			/ (double) rcPage.Width();

		// Find the Print height: Window height ratio
		double dHeightRatio = (double) m_rcPrintRect.Height() 
			/ (double) rcPage.Height();

		// Calculate the device's aspect ratio
		double dAspect = (double) pDC->GetDeviceCaps(ASPECTX) 
			/ (double) pDC->GetDeviceCaps(ASPECTY);

		// Find the new relative height
		int nHeight = (int) (rcPage.Height() *dWidthRatio * dAspect ); // 

		// Find the new relative width
		int nWidth = (int) (rcPage.Width() * dHeightRatio * ( 1.0 / dAspect ) );

		// Set the whole rectangle
		rcPage = m_rcPrintRect;

		// Determine the best fit, across or down the page
		if ( nHeight > nWidth )
		{
			// Down is best, so adjust the width
			rcPage.BottomRight().x = m_rcPrintRect.TopLeft().x + nWidth;
		}
		else 
		{
			// Across is best, so adjust the height
			rcPage.BottomRight().y = m_rcPrintRect.TopLeft().y + nHeight;
		}
	}
	// Convert to logical units
	 pDC->DPtoLP(&rcPage);


	// 1 - Prepare page footer
	CString strFooter;
	int nPageCount = pInfo->GetToPage() - pInfo->GetFromPage() + 1;

	strFooter.Format( _T("Page %d of %d" ), pInfo->m_nCurPage, nPageCount);

	CSize sizeFooter = pDC->GetTextExtent( strFooter );

	// 2 - Draw line
	int nBottomY = rcPage.bottom + BOTTOM_MARGIN + sizeFooter.cy * 2;

	int nCurrentY = nBottomY;
	pDC->MoveTo(0, nCurrentY );
	pDC->LineTo( rcPage.right, nCurrentY );
	nCurrentY -= sizeFooter.cy;

	// 3 - Draw footer
	CFont* pFontOld = 0;
	pFontOld = pDC->SelectObject(&m_PrintFont); 
	pDC->TextOutW( rcPage.left + ( rcPage.Width() - sizeFooter.cx ) / 2, nCurrentY, 
		strFooter );

	// 4 - Adjust remaining printable area
	rcPage.bottom = nBottomY;
	pDC->SelectObject(pFontOld);
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
	// TODO: Add your specialized code here and/or call the base class
	// Make sure the hint is valid
	
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
