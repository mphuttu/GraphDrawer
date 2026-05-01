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
#include <functional>
#include <limits>

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

	// Coordinate range (default ±10 math units)
	m_DrawOptionsData.dXMin = -10.0;
	m_DrawOptionsData.dXMax =  10.0;
	m_DrawOptionsData.dYMin = -10.0;
	m_DrawOptionsData.dYMax =  10.0;
	// Scale mode defaults
	m_DrawOptionsData.scaleMode = 0;  // equal scale
	m_DrawOptionsData.scaleX    = 1.0;
	m_DrawOptionsData.scaleY    = 1.0;
	m_DrawOptionsData.logMode   = 1;  // default sub-mode: Linear X, Log Y

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

// File format version — bump when adding new fields (use the loading 'version'
// guard below so old files continue to load with sensible defaults).
static const DWORD GD_FILE_VERSION = 1;

void CGraphDrawerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << GD_FILE_VERSION;

		// --- DrawOptionsData ---
		ar << m_DrawOptionsData.m_bCheckStateShowCoordinateAxes;
		ar << m_DrawOptionsData.m_bCheckStateShowTicks;
		ar << m_DrawOptionsData.m_bCheckStateShowTicksLabel;
		ar << m_DrawOptionsData.m_nTicksInterval;
		ar << m_DrawOptionsData.nIndexThickness;
		ar << m_DrawOptionsData.strCoordAxesThickness;
		ar << (DWORD)m_DrawOptionsData.clrCoordColor;
		ar << (DWORD)m_DrawOptionsData.clrBkgndColor;
		ar << m_DrawOptionsData.dXMin;
		ar << m_DrawOptionsData.dXMax;
		ar << m_DrawOptionsData.dYMin;
		ar << m_DrawOptionsData.dYMax;
		ar << m_DrawOptionsData.scaleMode;
		ar << m_DrawOptionsData.scaleX;
		ar << m_DrawOptionsData.scaleY;
		ar << m_DrawOptionsData.logMode;

		// --- Doc-level display state (used directly by the view) ---
		ar << m_bShowCoordinateAxes;
		ar << m_bShowTicks;
		ar << m_bShowTicksLabel;
		ar << m_nTicksInterval;
		ar << (DWORD)m_clrAxesColor;
		ar << m_nAxesPenWidth;
		ar << (DWORD)m_clrBkgndColor;

		// --- Built-in function visibility flags ---
		ar << m_bDrawSine;
		ar << m_bDrawCosine;
		ar << m_bDrawTan;
		ar << m_bDrawCotan;
		ar << m_bDrawExp;
		ar << m_bDrawLN;
		ar << m_bDrawArcsine;
		ar << m_bDrawArccosine;
		ar << m_bDrawArctan;
		ar << m_bDrawArccotan;
		ar << m_bDrawHyperbolicSine;
		ar << m_bDrawHyperbolicCosine;
		ar << m_bDrawHyperbolicTan;
		ar << m_bDrawHyperbolicCotan;

		// --- Custom expression ---
		ar << m_bDrawCustomFunction;
		ar << m_strCustomExpression;
		ar << m_dCustomRangeStart;
		ar << m_dCustomRangeEnd;

		// --- User-defined curves (definition only; points are recomputed on load) ---
		{
			CSingleLock lock(&m_csUserCurves, TRUE);
			DWORD nCurves = (DWORD)m_vecUserCurves.size();
			ar << nCurves;
			for (const UserCurve& c : m_vecUserCurves)
			{
				ar << (int)c.type;
				ar << c.label;
				ar << c.bVisible;
				ar << (DWORD)c.color;
				// y = f(x)
				ar << c.strExprY;
				ar << c.xStart;
				ar << c.xEnd;
				// parametric
				ar << c.strExprX;
				ar << c.strExprYPar;
				ar << c.tStart;
				ar << c.tEnd;
				// polar
				ar << c.strExprPolar;
				ar << c.phiStart;
				ar << c.phiEnd;
				// implicit
				ar << c.strExprImplicit;
				ar << c.xStartImp;
				ar << c.xEndImp;
				ar << c.yStartImp;
				ar << c.yEndImp;
			}
		}
	}
	else
	{
		DWORD version = 0;
		ar >> version;
		if (version > GD_FILE_VERSION)
		{
			AfxMessageBox(_T("This file was created with a newer version of GraphDrawer and cannot be opened."));
			return;
		}

		// --- DrawOptionsData ---
		ar >> m_DrawOptionsData.m_bCheckStateShowCoordinateAxes;
		ar >> m_DrawOptionsData.m_bCheckStateShowTicks;
		ar >> m_DrawOptionsData.m_bCheckStateShowTicksLabel;
		ar >> m_DrawOptionsData.m_nTicksInterval;
		ar >> m_DrawOptionsData.nIndexThickness;
		ar >> m_DrawOptionsData.strCoordAxesThickness;
		DWORD dw;
		ar >> dw; m_DrawOptionsData.clrCoordColor = (COLORREF)dw;
		ar >> dw; m_DrawOptionsData.clrBkgndColor = (COLORREF)dw;
		ar >> m_DrawOptionsData.dXMin;
		ar >> m_DrawOptionsData.dXMax;
		ar >> m_DrawOptionsData.dYMin;
		ar >> m_DrawOptionsData.dYMax;
		ar >> m_DrawOptionsData.scaleMode;
		ar >> m_DrawOptionsData.scaleX;
		ar >> m_DrawOptionsData.scaleY;
		ar >> m_DrawOptionsData.logMode;

		// --- Doc-level display state ---
		ar >> m_bShowCoordinateAxes;
		ar >> m_bShowTicks;
		ar >> m_bShowTicksLabel;
		ar >> m_nTicksInterval;
		ar >> dw; m_clrAxesColor = (COLORREF)dw;
		ar >> m_nAxesPenWidth;
		ar >> dw; m_clrBkgndColor = (COLORREF)dw;

		// --- Built-in function visibility flags ---
		ar >> m_bDrawSine;
		ar >> m_bDrawCosine;
		ar >> m_bDrawTan;
		ar >> m_bDrawCotan;
		ar >> m_bDrawExp;
		ar >> m_bDrawLN;
		ar >> m_bDrawArcsine;
		ar >> m_bDrawArccosine;
		ar >> m_bDrawArctan;
		ar >> m_bDrawArccotan;
		ar >> m_bDrawHyperbolicSine;
		ar >> m_bDrawHyperbolicCosine;
		ar >> m_bDrawHyperbolicTan;
		ar >> m_bDrawHyperbolicCotan;

		// --- Custom expression ---
		ar >> m_bDrawCustomFunction;
		ar >> m_strCustomExpression;
		ar >> m_dCustomRangeStart;
		ar >> m_dCustomRangeEnd;

		// --- User-defined curves ---
		DWORD nCurves = 0;
		ar >> nCurves;
		{
			CSingleLock lock(&m_csUserCurves, TRUE);
			m_vecUserCurves.clear();
			m_vecUserCurves.reserve(nCurves);
			for (DWORD i = 0; i < nCurves; ++i)
			{
				UserCurve c;
				int iType;
				ar >> iType; c.type = (UserCurveType)iType;
				ar >> c.label;
				ar >> c.bVisible;
				ar >> dw; c.color = (COLORREF)dw;
				// y = f(x)
				ar >> c.strExprY;
				ar >> c.xStart;
				ar >> c.xEnd;
				// parametric
				ar >> c.strExprX;
				ar >> c.strExprYPar;
				ar >> c.tStart;
				ar >> c.tEnd;
				// polar
				ar >> c.strExprPolar;
				ar >> c.phiStart;
				ar >> c.phiEnd;
				// implicit
				ar >> c.strExprImplicit;
				ar >> c.xStartImp;
				ar >> c.xEndImp;
				ar >> c.yStartImp;
				ar >> c.yEndImp;
				m_vecUserCurves.push_back(std::move(c));
			}
		}

		// Recompute curve points from the loaded expressions.
		RecomputeUserCurves();

		// Restart the custom-expression background thread if needed.
		if (m_bDrawCustomFunction && !m_strCustomExpression.IsEmpty())
			SetCustomExpression(m_strCustomExpression, m_bDrawCustomFunction,
			                    m_dCustomRangeStart, m_dCustomRangeEnd);
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

		// Recompute user-curve points if the range changed.
		RecomputeUserCurves();
	}
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL, 1);  // hint=1 signals a range change (view resets zoom/pan)
}

void DrawCoordinateAxes(CDC* pDC, BOOL bShowTicks, int nTicksInterval,
						BOOL bShowLabels, const CoordTransform& ct)
{
	// Origin = math (0,0)
	CPoint ORIGIN = ct.ToLogical(0.0, 0.0);

	double scaleX = ct.ScaleX();  // logical units per math unit
	double scaleY = ct.ScaleY();

	// Puolilogaritmisessa tilassa (bLogY=true, bLogX=false):
	// Y=0 ei ole näkyvissä, joten X-akseli piirretään yMin-reunaan,
	// Y-akseli piirretään normaalisti x=0:ssa (tai xMin-reunassa jos 0 ei näy).
	double xAxisY  = (ct.bLogY)  ? ct.yMin : 0.0;  // X-akselin Y-arvo
	double yAxisX  = (ct.bLogX)  ? ct.xMin : 0.0;  // Y-akselin X-arvo

	// Clipped ends of axes
	CPoint axisXLeft  = ct.ToLogical(ct.xMin, xAxisY);
	CPoint axisXRight = ct.ToLogical(ct.xMax, xAxisY);
	CPoint axisYBot   = ct.ToLogical(yAxisX, ct.yMin);
	CPoint axisYTop   = ct.ToLogical(yAxisX, ct.yMax);

	// X Axis
	pDC->MoveTo(axisXLeft);
	pDC->LineTo(axisXRight);
	pDC->TextOutW(axisXRight.x + 4, axisXRight.y + 10, _T("X"));

	// Y Axis
	pDC->MoveTo(axisYBot);
	pDC->LineTo(axisYTop);
	pDC->TextOutW(axisYTop.x + 4, axisYTop.y - 10, _T("Y"));

	if (!bShowTicks || nTicksInterval <= 0)
		return;

	const int tickHalf = 5;  // logical units

	// -----------------------------------------------------------------------
	// Logaritminen asteikko: tikut desikaadeittain (10^n) ja välit (2..9)
	// -----------------------------------------------------------------------
	if (ct.bLogX || ct.bLogY)
	{
		// X-akseli log-tikut (jos X on log)
		if (ct.bLogX && ct.xMin > 0 && ct.xMax > ct.xMin)
		{
			int expMin = (int)std::floor(std::log10(ct.xMin));
			int expMax = (int)std::ceil(std::log10(ct.xMax));
			for (int e = expMin; e <= expMax; ++e)
			{
				double base = std::pow(10.0, e);
				if (base >= ct.xMin && base <= ct.xMax)
				{
					CPoint tp = ct.ToLogical(base, xAxisY);
					pDC->MoveTo(tp.x, tp.y - tickHalf * 2);
					pDC->LineTo(tp.x, tp.y + tickHalf * 2);
					if (bShowLabels)
					{
						CString s;
						s.Format(_T("1e%d"), e);
						pDC->TextOutW(tp.x - 8, tp.y + tickHalf * 2 + 2, s);
					}
				}
				for (int m = 2; m <= 9; ++m)
				{
					double tx = m * base;
					if (tx < ct.xMin || tx > ct.xMax) continue;
					CPoint tp = ct.ToLogical(tx, xAxisY);
					pDC->MoveTo(tp.x, tp.y - tickHalf);
					pDC->LineTo(tp.x, tp.y + tickHalf);
				}
			}
		}
		// Puolilog: X lineaarinen tikut X-akselille (xAxisY-kohtaan)
		if (!ct.bLogX)
		{
			double xRange2 = ct.xMax - ct.xMin;
			auto niceStep2 = [](double range) -> double {
				if (range <= 0.0) return 1.0;
				double rawStep = range / 8.0;
				double magnitude = std::pow(10.0, std::floor(std::log10(rawStep)));
				double norm = rawStep / magnitude;
				return ((norm < 1.5) ? 1.0 : (norm < 3.5) ? 2.0 : (norm < 7.5) ? 5.0 : 10.0) * magnitude;
			};
			double stepX2 = niceStep2(xRange2);
			double firstX2 = std::ceil(ct.xMin / stepX2) * stepX2;
			for (double tx = firstX2; tx <= ct.xMax + 1e-9; tx += stepX2)
			{
				CPoint tp = ct.ToLogical(tx, xAxisY);
				pDC->MoveTo(tp.x, tp.y - tickHalf);
				pDC->LineTo(tp.x, tp.y + tickHalf);
				if (bShowLabels)
				{
					CString s;
					if (std::abs(tx - std::round(tx)) < 1e-9)
						s.Format(_T("%g"), std::round(tx));
					else
						s.Format(_T("%g"), tx);
					pDC->TextOutW(tp.x - 8, tp.y + tickHalf + 2, s);
				}
			}
		}
		// Y-akseli log-tikut
		if (ct.bLogY && ct.yMin > 0 && ct.yMax > ct.yMin)
		{
			int expMin = (int)std::floor(std::log10(ct.yMin));
			int expMax = (int)std::ceil(std::log10(ct.yMax));
			for (int e = expMin; e <= expMax; ++e)
			{
				double base = std::pow(10.0, e);
				if (base >= ct.yMin && base <= ct.yMax)
				{
					CPoint tp = ct.ToLogical(yAxisX, base);
					pDC->MoveTo(tp.x - tickHalf * 2, tp.y);
					pDC->LineTo(tp.x + tickHalf * 2, tp.y);
					if (bShowLabels)
					{
						CString s;
						s.Format(_T("1e%d"), e);
						pDC->TextOutW(tp.x + tickHalf * 2 + 3, tp.y, s);
					}
				}
				for (int m = 2; m <= 9; ++m)
				{
					double ty = m * base;
					if (ty < ct.yMin || ty > ct.yMax) continue;
					CPoint tp = ct.ToLogical(yAxisX, ty);
					pDC->MoveTo(tp.x - tickHalf, tp.y);
					pDC->LineTo(tp.x + tickHalf, tp.y);
				}
			}
		}
		// Lineaariset Y-tikut kun Y ei ole logaritminen (LogX + LinY -tila)
		if (!ct.bLogY)
		{
			double yRange2 = ct.yMax - ct.yMin;
			auto niceStepY = [](double range) -> double {
				if (range <= 0.0) return 1.0;
				double rawStep = range / 8.0;
				double magnitude = std::pow(10.0, std::floor(std::log10(rawStep)));
				double norm = rawStep / magnitude;
				return ((norm < 1.5) ? 1.0 : (norm < 3.5) ? 2.0 : (norm < 7.5) ? 5.0 : 10.0) * magnitude;
			};
			double stepY2 = niceStepY(yRange2);
			double firstY2 = std::ceil(ct.yMin / stepY2) * stepY2;
			for (double ty = firstY2; ty <= ct.yMax + 1e-9; ty += stepY2)
			{
				CPoint tp = ct.ToLogical(yAxisX, ty);
				pDC->MoveTo(tp.x - tickHalf, tp.y);
				pDC->LineTo(tp.x + tickHalf, tp.y);
				if (bShowLabels)
				{
					CString s;
					if (std::abs(ty - std::round(ty)) < 1e-9)
						s.Format(_T("%g"), std::round(ty));
					else
						s.Format(_T("%g"), ty);
					pDC->TextOutW(tp.x + tickHalf + 3, tp.y, s);
				}
			}
		}
		return;
	}

	// -----------------------------------------------------------------------
	// Lineaarinen asteikko (alkuperäinen logiikka)
	// -----------------------------------------------------------------------
	double xRange = ct.xMax - ct.xMin;
	double yRange = ct.yMax - ct.yMin;

	// Choose a tick step that gives ~5-10 ticks across the range.
	auto niceStep = [](double range) -> double {
		if (range <= 0.0) return 1.0;
		double rawStep = range / 8.0;
		double magnitude = std::pow(10.0, std::floor(std::log10(rawStep)));
		double norm = rawStep / magnitude;
		double step = (norm < 1.5) ? 1.0 : (norm < 3.5) ? 2.0 : (norm < 7.5) ? 5.0 : 10.0;
		return step * magnitude;
	};
	double tickStepX = niceStep(xRange);
	double tickStepY = niceStep(yRange);

	// Tasainen skaalaus (SCALE_EQUAL): ScaleX() == ScaleY() → 1 math-yksikkö =
	// sama pikseliä molemmilla akseleilla. Käytetään sama tikutusaskel molemmille
	// jotta tikut ovat fyysisesti yhtä tiheässä x- ja y-suunnassa.
	{
		double sx = ct.ScaleX(), sy = ct.ScaleY();
		double avg = (sx + sy) * 0.5;
		if (avg > 0.0 && std::abs(sx - sy) / avg < 0.01)
			tickStepX = tickStepY = (tickStepX < tickStepY ? tickStepX : tickStepY);
	}

	// X ticks
	double firstX = std::ceil(ct.xMin / tickStepX) * tickStepX;
	for (double tx = firstX; tx <= ct.xMax + 1e-9; tx += tickStepX)
	{
		CPoint tp = ct.ToLogical(tx, xAxisY);
		pDC->MoveTo(tp.x, tp.y - tickHalf);
		pDC->LineTo(tp.x, tp.y + tickHalf);
		if (bShowLabels)
		{
			CString s;
			if (std::abs(tx - std::round(tx)) < 1e-9)
				s.Format(_T("%g"), std::round(tx));
			else
				s.Format(_T("%g"), tx);
			pDC->TextOutW(tp.x - 8, tp.y + tickHalf + 2, s);
		}
	}

	// Y ticks
	double firstY = std::ceil(ct.yMin / tickStepY) * tickStepY;
	for (double ty = firstY; ty <= ct.yMax + 1e-9; ty += tickStepY)
	{
		CPoint tp = ct.ToLogical(yAxisX, ty);
		pDC->MoveTo(tp.x - tickHalf, tp.y);
		pDC->LineTo(tp.x + tickHalf, tp.y);
		if (bShowLabels)
		{
			CString s;
			if (std::abs(ty - std::round(ty)) < 1e-9)
				s.Format(_T("%g"), std::round(ty));
			else
				s.Format(_T("%g"), ty);
			pDC->TextOutW(tp.x + tickHalf + 3, tp.y, s);
		}
	}
}


CSize& CGraphDrawerDoc::GetDocSize(void)
{
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

		if (p->bLogX && p->xStart > 0.0 && p->xEnd > p->xStart)
		{
			// Log-avaruuden näytteenotto: tasavälinen log10-avaruudessa
			const int N = 80000;
			double logStart = std::log10(p->xStart);
			double logEnd   = std::log10(p->xEnd);
			double step     = (logEnd - logStart) / N;
			pts.reserve(N + 1);
			for (int i = 0; i <= N; ++i)
			{
				if (pDoc->m_bCancelDraw) break;
				double x = std::pow(10.0, logStart + i * step);
				double y = 0.0;
				if (parser.Evaluate(x, y))
					pts.push_back({ x, y });
			}
		}
		else
		{
			pts.reserve(static_cast<size_t>((p->xEnd - p->xStart) / p->xStep) + 1);
			for (double x = p->xStart; x <= p->xEnd; x += p->xStep)
			{
				if (pDoc->m_bCancelDraw)
					break;

				double y = 0.0;
				if (parser.Evaluate(x, y))
					pts.push_back({ x, y });
			}
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
	params->bLogX         = (m_DrawOptionsData.scaleMode == 2) &&
	                        (m_DrawOptionsData.logMode == 0 || m_DrawOptionsData.logMode == 2);
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

// ---------------------------------------------------------------------------
// Helper: draw a polyline from pre-computed MathPoints using CoordTransform.
// ---------------------------------------------------------------------------
static void DrawPolylinePts(CDC* pDC, const CoordTransform& ct,
                             const std::vector<CGraphDrawerDoc::MathPoint>& pts,
                             int absHalfH)
{
	bool first = true;
	CPoint prev;
	for (const auto& mp : pts)
	{
		CPoint cur = ct.ToLogical(mp.x, mp.y);
		if (cur.x < -32000 || cur.x > 32000 || cur.y < -32000 || cur.y > 32000)
		{
			first = true; continue;
		}
		if (first) { pDC->MoveTo(cur); first = false; }
		else {
			if (abs(cur.y - prev.y) > absHalfH * 2)
				pDC->MoveTo(cur);
			else
				pDC->LineTo(cur);
		}
		prev = cur;
	}
}

// Draw the pre-computed custom-function points onto the DC.
void DrawCustomFunction(CDC* pDC, const CoordTransform& ct, COLORREF /*clrColor*/,
	CGraphDrawerDoc* pDoc)
{
	if (!pDoc->m_bDrawCustomFunction)
		return;

	CSingleLock lock(&pDoc->m_csCustomPoints, TRUE);
	const auto& pts = pDoc->m_vecCustomPoints;
	if (pts.empty())
		return;

	DrawPolylinePts(pDC, ct,
		reinterpret_cast<const std::vector<CGraphDrawerDoc::MathPoint>&>(pts),
		ct.clientH / 2);
}

// Draw all user-defined curves.
void DrawUserCurves(CDC* pDC, const CoordTransform& ct,
                    std::vector<UserCurve>& vecCurves, CCriticalSection& cs)
{
	CSingleLock lock(&cs, TRUE);
	for (const auto& curve : vecCurves)
	{
		if (!curve.bVisible)
			continue;

		CPen pen;
		if (!pen.CreatePen(PS_SOLID, 1, curve.color))
			continue;
		CPen* pOldPen = pDC->SelectObject(&pen);

		if (curve.type == UCT_IMPLICIT) {
			// Piirrä jokainen segmentti
			for (const auto& seg : curve.segments) {
				CPoint p1 = ct.ToLogical(seg.first.x, seg.first.y);
				CPoint p2 = ct.ToLogical(seg.second.x, seg.second.y);
				// Suodatetaan pois kaukana olevat
				if ((p1.x < -32000 || p1.x > 32000 || p1.y < -32000 || p1.y > 32000) ||
					(p2.x < -32000 || p2.x > 32000 || p2.y < -32000 || p2.y > 32000))
					continue;
				pDC->MoveTo(p1);
				pDC->LineTo(p2);
			}
		} else if (!curve.points.empty()) {
			bool first = true;
			CPoint prev;
			for (const auto& mp : curve.points)
			{
				CPoint cur = ct.ToLogical(mp.x, mp.y);
				if (cur.x < -32000 || cur.x > 32000 || cur.y < -32000 || cur.y > 32000)
				{
					first = true; continue;
				}
				if (first) { pDC->MoveTo(cur); first = false; }
				else {
					if (abs(cur.y - prev.y) > ct.clientH * 2)
						pDC->MoveTo(cur);
					else
						pDC->LineTo(cur);
				}
				prev = cur;
			}
		}
		pDC->SelectObject(pOldPen);
	}
}

// ---------------------------------------------------------------------------
// Helper: sample a function over [xMin, xMax] using the expression parser.
// ---------------------------------------------------------------------------
static void SampleYFX(const CString& expr, double xStart, double xEnd,
                      std::vector<UserCurve::MathPoint>& out, bool bLogX = false)
{
	CExpressionParser parser;
	if (!parser.Parse(expr))
		return;

	const int N = 10000;
	out.reserve(N + 1);
	if (bLogX && xStart > 0.0 && xEnd > xStart)
	{
		// Tasavälinen näytteenotto log-avaruudessa
		double logStart = std::log10(xStart);
		double logEnd   = std::log10(xEnd);
		double step = (logEnd - logStart) / N;
		for (int i = 0; i <= N; ++i)
		{
			double x = std::pow(10.0, logStart + i * step);
			double y = 0.0;
			if (parser.Evaluate(x, y))
				out.push_back({ x, y });
		}
	}
	else
	{
		double step = (xEnd - xStart) / N;
		for (int i = 0; i <= N; ++i)
		{
			double x = xStart + i * step;
			double y = 0.0;
			if (parser.Evaluate(x, y))
				out.push_back({ x, y });
		}
	}
}

static void SampleParametric(const CString& exprX, const CString& exprY,
                              double tStart, double tEnd,
                              std::vector<UserCurve::MathPoint>& out)
{
	CExpressionParser parserX, parserY;
	if (!parserX.Parse(exprX) || !parserY.Parse(exprY))
		return;

	const int N = 10000;
	double step = (tEnd - tStart) / N;
	out.reserve(N + 1);
	for (int i = 0; i <= N; ++i)
	{
		double t = tStart + i * step;
		double x = 0.0, y = 0.0;
		if (parserX.EvaluateT(t, x) && parserY.EvaluateT(t, y))
			out.push_back({ x, y });
	}
}

static void SamplePolar(const CString& exprR, double kStart, double kEnd,
                        std::vector<UserCurve::MathPoint>& out)
{
	CExpressionParser parser;
	if (!parser.Parse(exprR))
		return;

	const int N = 10000;
	double step = (kEnd - kStart) / N;
	out.reserve(N + 1);
	for (int i = 0; i <= N; ++i)
	{
		double k = kStart + i * step;
		double r = 0.0;
		if (parser.Evaluate(k, r))
		{
			double x = r * std::cos(k);
			double y = r * std::sin(k);
			out.push_back({ x, y });
		}
	}
}

// MathPoint-segmenttipari
using Segment = std::pair<UserCurve::MathPoint, UserCurve::MathPoint>;

static void SampleImplicit(const CString& exprF, double x0, double x1, double y0, double y1, std::vector<Segment>& out)
{
	CExpressionParser parser;
	if (!parser.Parse(exprF))
		return;
	const int NX = 400, NY = 400;
	double dx = (x1 - x0) / NX;
	double dy = (y1 - y0) / NY;
	std::vector<std::vector<double>> F(NX+1, std::vector<double>(NY+1));
	for (int i = 0; i <= NX; ++i)
	{
		double x = x0 + i * dx;
		for (int j = 0; j <= NY; ++j)
		{
			double y = y0 + j * dy;
			double f = 0.0;
			parser.Evaluate(x, y, f);
			F[i][j] = f;
		}
	}
	for (int i = 0; i < NX; ++i)
	{
		double x00 = x0 + i * dx;
		double x10 = x0 + (i+1) * dx;
		for (int j = 0; j < NY; ++j)
		{
			double y00 = y0 + j * dy;
			double y01 = y0 + (j+1) * dy;
			double F00 = F[i][j];
			double F10 = F[i+1][j];
			double F11 = F[i+1][j+1];
			double F01 = F[i][j+1];
			int code = (F00 < 0) | ((F10 < 0) << 1) | ((F11 < 0) << 2) | ((F01 < 0) << 3);
			if (code == 0 || code == 15) continue;
			// Interpoloi reunat
			UserCurve::MathPoint pts[4];
			int n = 0;
			auto interp = [](double f1, double f2, double x1, double y1, double x2, double y2) {
				double t = f1 / (f1 - f2);
				return UserCurve::MathPoint{ x1 + t * (x2 - x1), y1 + t * (y2 - y1) };
			};
			if ((F00 < 0) != (F01 < 0)) pts[n++] = interp(F00, F01, x00, y00, x00, y01);
			if ((F10 < 0) != (F11 < 0)) pts[n++] = interp(F10, F11, x10, y00, x10, y01);
			if ((F00 < 0) != (F10 < 0)) pts[n++] = interp(F00, F10, x00, y00, x10, y00);
			if ((F01 < 0) != (F11 < 0)) pts[n++] = interp(F01, F11, x00, y01, x10, y01);
			if (n == 2) out.push_back({ pts[0], pts[1] });
			else if (n == 4) { out.push_back({ pts[0], pts[1] }); out.push_back({ pts[2], pts[3] }); }
		}
	}
}

// ---------------------------------------------------------------------------
// CGraphDrawerDoc user-curve management
// ---------------------------------------------------------------------------
void CGraphDrawerDoc::AddUserCurve(const UserCurve& curveDef)
{
	const bool bLogX = (m_DrawOptionsData.scaleMode == 2) &&
	                   (m_DrawOptionsData.logMode == 0 || m_DrawOptionsData.logMode == 2);
	UserCurve c = curveDef;
	c.points.clear();
	c.segments.clear();
	if (c.type == UCT_YFX)
		SampleYFX(c.strExprY, c.xStart, c.xEnd, c.points, bLogX);
	else if (c.type == UCT_PARAMETRIC)
		SampleParametric(c.strExprX, c.strExprYPar, c.tStart, c.tEnd, c.points);
	else if (c.type == UCT_POLAR)
		SamplePolar(c.strExprPolar, c.phiStart, c.phiEnd, c.points);
	else if (c.type == UCT_IMPLICIT)
		SampleImplicit(c.strExprImplicit, c.xStartImp, c.xEndImp, c.yStartImp, c.yEndImp, c.segments);
	{
		CSingleLock lock(&m_csUserCurves, TRUE);
		m_vecUserCurves.push_back(std::move(c));
	}
	UpdateAllViews(NULL);
}

void CGraphDrawerDoc::RemoveUserCurve(int idx)
{
	CSingleLock lock(&m_csUserCurves, TRUE);
	if (idx >= 0 && idx < (int)m_vecUserCurves.size())
		m_vecUserCurves.erase(m_vecUserCurves.begin() + idx);
	lock.Unlock();
	UpdateAllViews(NULL);
}

void CGraphDrawerDoc::SetUserCurveVisible(int idx, BOOL bVisible)
{
	{
		CSingleLock lock(&m_csUserCurves, TRUE);
		if (idx >= 0 && idx < (int)m_vecUserCurves.size())
			m_vecUserCurves[idx].bVisible = bVisible;
	}
	UpdateAllViews(NULL);
}

void CGraphDrawerDoc::RecomputeUserCurves()
{
	const bool bLogX = (m_DrawOptionsData.scaleMode == 2) &&
	                   (m_DrawOptionsData.logMode == 0 || m_DrawOptionsData.logMode == 2);
	CSingleLock lock(&m_csUserCurves, TRUE);
	for (auto& c : m_vecUserCurves)
	{
		c.points.clear();
		c.segments.clear();
		if (c.type == UCT_YFX)
			SampleYFX(c.strExprY, c.xStart, c.xEnd, c.points, bLogX);
		else if (c.type == UCT_PARAMETRIC)
			SampleParametric(c.strExprX, c.strExprYPar, c.tStart, c.tEnd, c.points);
		else if (c.type == UCT_POLAR)
			SamplePolar(c.strExprPolar, c.phiStart, c.phiEnd, c.points);
		else if (c.type == UCT_IMPLICIT)
			SampleImplicit(c.strExprImplicit, c.xStartImp, c.xEndImp, c.yStartImp, c.yEndImp, c.segments);
	}
}

// ---------------------------------------------------------------------------
// Built-in function curves — all refactored to use CoordTransform
// ---------------------------------------------------------------------------

// Generic helper: draw f(x) using SetPixel over the visible x range.
static void DrawFuncPixels(CDC* pDC, const CoordTransform& ct, COLORREF color,
                            std::function<double(double)> fn)
{
	// Use a polyline approach (MoveTo/LineTo) instead of SetPixel — much faster
	// and produces anti-aliased-looking connected curves instead of isolated dots.
	CPen pen(PS_SOLID, 1, color);
	CPen* pOldPen = pDC->SelectObject(&pen);

	const double step = (ct.xMax - ct.xMin) / (ct.clientW > 0 ? ct.clientW : 1000);
	bool first = true;
	CPoint prev;
	for (double x = ct.xMin; x <= ct.xMax + step * 0.5; x += step)
	{
		double y = fn(x);
		if (!_finite(y) || _isnan(y)) { first = true; continue; }
		CPoint p = ct.ToLogical(x, y);
		if (p.x < -32000 || p.x > 32000 || p.y < -32000 || p.y > 32000)
		{ first = true; continue; }
		if (first) { pDC->MoveTo(p); first = false; }
		else {
			if (abs(p.y - prev.y) > ct.clientH * 2) pDC->MoveTo(p);
			else pDC->LineTo(p);
		}
		prev = p;
	}
	pDC->SelectObject(pOldPen);
}

void DrawSine(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){ return std::sin(x); });
}

void DrawCosine(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){ return std::cos(x); });
}

void DrawTan(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){
		double c = std::cos(x);
		if (std::abs(c) < 1e-6) return std::numeric_limits<double>::quiet_NaN();
		return std::tan(x);
	});
}

void DrawCotan(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){
		double s = std::sin(x);
		if (std::abs(s) < 1e-6) return std::numeric_limits<double>::quiet_NaN();
		return std::cos(x) / s;
	});
}

void DrawExp(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){ return std::exp(x); });
}

void DrawLN(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){
		if (x <= 0.0) return std::numeric_limits<double>::quiet_NaN();
		return std::log(x);
	});
}

void DrawArcsine(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){
		if (x < -1.0 || x > 1.0) return std::numeric_limits<double>::quiet_NaN();
		return std::asin(x);
	});
}

void DrawArccosine(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){
		if (x < -1.0 || x > 1.0) return std::numeric_limits<double>::quiet_NaN();
		return std::acos(x);
	});
}

void DrawArctan(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){ return std::atan(x); });
}

double cotan(double x)  { return 1.0 / std::tan(x); }
double arccotan(double y) { return std::atan(1.0 / y); }

void DrawArccotan(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){
		if (std::abs(x) < 1e-10) return std::numeric_limits<double>::quiet_NaN();
		return std::atan(1.0 / x);
	});
}

void DrawHyperbolicSine(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){ return std::sinh(x); });
}

void DrawHyperbolicCosine(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){ return std::cosh(x); });
}

void DrawHyperbolicTan(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){ return std::tanh(x); });
}

double coth(double x) { return 1.0 / std::tanh(x); }

void DrawHyperbolicCotan(CDC* pDC, const CoordTransform& ct, COLORREF clrColor)
{
	DrawFuncPixels(pDC, ct, clrColor, [](double x){
		if (std::abs(x) < 1e-10) return std::numeric_limits<double>::quiet_NaN();
		return 1.0 / std::tanh(x);
	});
}

