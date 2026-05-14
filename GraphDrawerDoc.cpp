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
#include <algorithm>

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
		delete m_pDrawThread;   // safe: m_bAutoDelete = FALSE
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
static const DWORD GD_FILE_VERSION = 2;

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

		// --- Geometric objects (version 2+) ---
		{
			CSingleLock lock(&m_csGeoObjects, TRUE);
			DWORD nGeo = (DWORD)m_vecGeoObjects.size();
			ar << nGeo;
			for (const GeoObject& obj : m_vecGeoObjects)
			{
				ar << (int)obj.type;
				if (obj.type == GOT_LINE_SEGMENT)
				{
					const GeoLineSegment& ln = obj.line;
					ar << ln.x1 << ln.y1 << ln.x2 << ln.y2;
					ar << ln.label;
					ar << (DWORD)ln.color;
					ar << ln.bVisible;
					ar << ln.bParallel;
					ar << ln.dParallelOffset;
					ar << ln.bTransversal;
					ar << ln.dTransversalX << ln.dTransversalY;
					ar << ln.dTransversalAngleDeg;
				}
				else
				{
					const GeoTriangle& tri = obj.triangle;
					ar << tri.ax << tri.ay << tri.bx << tri.by << tri.cx << tri.cy;
					ar << tri.labelA << tri.labelB << tri.labelC;
					ar << tri.labelSideA << tri.labelSideB << tri.labelSideC;
					ar << tri.labelAngleA << tri.labelAngleB << tri.labelAngleC;
					ar << tri.bShowVertexLabels << tri.bShowSideLabels;
					ar << tri.bShowAngleLabels << tri.bShowAngleValues;
					ar << tri.bCircumcircle << tri.bIncircle;
					ar << tri.bPerpBisectors << tri.bAngleBisectors;
					ar << tri.bAltitudes << tri.bMedians;
					ar << (int)tri.type;
					ar << tri.label;
					ar << (DWORD)tri.color;
					ar << tri.bVisible;
				}
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

		// --- Geometric objects (version 2+) ---
		if (version >= 2)
		{
			DWORD nGeo = 0;
			ar >> nGeo;
			CSingleLock lock(&m_csGeoObjects, TRUE);
			m_vecGeoObjects.clear();
			m_vecGeoObjects.reserve(nGeo);
			for (DWORD i = 0; i < nGeo; ++i)
			{
				GeoObject obj;
				int iType; ar >> iType; obj.type = (GeoObjectType)iType;
				if (obj.type == GOT_LINE_SEGMENT)
				{
					GeoLineSegment& ln = obj.line;
					ar >> ln.x1 >> ln.y1 >> ln.x2 >> ln.y2;
					ar >> ln.label;
					ar >> dw; ln.color = (COLORREF)dw;
					ar >> ln.bVisible;
					ar >> ln.bParallel;
					ar >> ln.dParallelOffset;
					ar >> ln.bTransversal;
					ar >> ln.dTransversalX >> ln.dTransversalY;
					ar >> ln.dTransversalAngleDeg;
				}
				else
				{
					GeoTriangle& tri = obj.triangle;
					ar >> tri.ax >> tri.ay >> tri.bx >> tri.by >> tri.cx >> tri.cy;
					ar >> tri.labelA >> tri.labelB >> tri.labelC;
					ar >> tri.labelSideA >> tri.labelSideB >> tri.labelSideC;
					ar >> tri.labelAngleA >> tri.labelAngleB >> tri.labelAngleC;
					ar >> tri.bShowVertexLabels >> tri.bShowSideLabels;
					ar >> tri.bShowAngleLabels >> tri.bShowAngleValues;
					ar >> tri.bCircumcircle >> tri.bIncircle;
					ar >> tri.bPerpBisectors >> tri.bAngleBisectors;
					ar >> tri.bAltitudes >> tri.bMedians;
					int iTri; ar >> iTri; tri.type = (TriangleType)iTri;
					ar >> tri.label;
					ar >> dw; tri.color = (COLORREF)dw;
					ar >> tri.bVisible;
				}
				m_vecGeoObjects.push_back(std::move(obj));
			}
		}
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
		delete m_pDrawThread;   // safe: m_bAutoDelete = FALSE
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
		// Keep m_bAutoDelete = FALSE so we own the CWinThread object and can
		// safely call WaitForSingleObject + delete without a use-after-free.
		m_pDrawThread->m_bAutoDelete = FALSE;
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
// DrawGeometricObjects — draw all geometric figures stored in the document
// ---------------------------------------------------------------------------

// Forward declarations of helper drawing functions
static void DrawGeoLineSegment(CDC* pDC, const CoordTransform& ct, const GeoLineSegment& seg);
static void DrawGeoTriangle(CDC* pDC, const CoordTransform& ct, const GeoTriangle& tri);

void DrawGeometricObjects(CDC* pDC, const CoordTransform& ct, CGraphDrawerDoc* pDoc)
{
	CSingleLock lock(&pDoc->m_csGeoObjects, TRUE);
	for (const auto& obj : pDoc->m_vecGeoObjects)
	{
		if (!obj.IsVisible()) continue;
		if (obj.type == GOT_LINE_SEGMENT)
			DrawGeoLineSegment(pDC, ct, obj.line);
		else
			DrawGeoTriangle(pDC, ct, obj.triangle);
	}
}

// ---------------------------------------------------------------------------
// Helper: draw a text label at math coordinate, with slight offset in pixels
// ---------------------------------------------------------------------------
static void DrawLabel(CDC* pDC, const CoordTransform& ct,
                      double mathX, double mathY,
                      const CString& text,
                      int offX = 4, int offY = -14)
{
	if (text.IsEmpty()) return;
	CPoint p = ct.ToLogical(mathX, mathY);
	UINT oldAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
	int oldBk = pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(p.x + offX, p.y + offY, text);
	pDC->SetTextAlign(oldAlign);
	pDC->SetBkMode(oldBk);
}

// ---------------------------------------------------------------------------
// DrawGeoLineSegment
// ---------------------------------------------------------------------------
static void DrawGeoLineSegment(CDC* pDC, const CoordTransform& ct, const GeoLineSegment& seg)
{
	CPen pen(PS_SOLID, 2, seg.color);
	CPen* pOld = pDC->SelectObject(&pen);

	CPoint p1 = ct.ToLogical(seg.x1, seg.y1);
	CPoint p2 = ct.ToLogical(seg.x2, seg.y2);
	pDC->MoveTo(p1);
	pDC->LineTo(p2);

	// Draw small circles at endpoints
	pDC->Ellipse(p1.x - 3, p1.y - 3, p1.x + 3, p1.y + 3);
	pDC->Ellipse(p2.x - 3, p2.y - 3, p2.x + 3, p2.y + 3);

	// Draw label near midpoint
	if (!seg.label.IsEmpty())
	{
		double mx = (seg.x1 + seg.x2) * 0.5;
		double my = (seg.y1 + seg.y2) * 0.5;
		DrawLabel(pDC, ct, mx, my, seg.label, 4, -14);
	}

	double dx = seg.x2 - seg.x1;
	double dy = seg.y2 - seg.y1;
	double len = std::sqrt(dx*dx + dy*dy);

	// Parallel line
	if (seg.bParallel && len > 1e-10)
	{
		double nx = -dy / len;
		double ny =  dx / len;
		double off = seg.dParallelOffset;

		CPen parPen(PS_DASH, 1, seg.color);
		pDC->SelectObject(&parPen);

		CPoint p3 = ct.ToLogical(seg.x1 + nx*off, seg.y1 + ny*off);
		CPoint p4 = ct.ToLogical(seg.x2 + nx*off, seg.y2 + ny*off);
		pDC->MoveTo(p3);
		pDC->LineTo(p4);
		pDC->SelectObject(&pen);  // restore solid pen
	}

	// Transversal line
	if (seg.bTransversal)
	{
		double angRad = seg.dTransversalAngleDeg * M_PI / 180.0;
		double cosA = std::cos(angRad), sinA = std::sin(angRad);
		double extent = (len > 1.0 ? len : 1.0) * 3.0
		              + (seg.bParallel ? std::abs(seg.dParallelOffset) : 0.0)
		              + 2.0;

		double tx = seg.dTransversalX;
		double ty = seg.dTransversalY;

		double t1x = tx - cosA * extent, t1y = ty - sinA * extent;
		double t2x = tx + cosA * extent, t2y = ty + sinA * extent;

		CPen transPen(PS_DOT, 1, seg.color);
		pDC->SelectObject(&transPen);
		CPoint pt1 = ct.ToLogical(t1x, t1y);
		CPoint pt2 = ct.ToLogical(t2x, t2y);
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);

		// --- Compute intersection with the main line and draw angle label ---
		// Line 1: P1 + s*(P2-P1),  Line 2: T + t*(dir)
		// P1=(x1,y1), dir1=(dx,dy), T=(tx,ty), dir2=(cosA,sinA)
		// Solve: x1 + s*dx = tx + t*cosA
		//        y1 + s*dy = ty + t*sinA
		double denom = dx * sinA - dy * cosA;
		if (std::abs(denom) > 1e-10)
		{
			double s = ((tx - seg.x1)*sinA - (ty - seg.y1)*cosA) / denom;
			double ix = seg.x1 + s*dx;
			double iy = seg.y1 + s*dy;

			// Angle between main line and transversal
			double dot = dx/len * cosA + dy/len * sinA;
			dot = std::max(-1.0, std::min(1.0, dot));
			double angleDeg = std::acos(std::abs(dot)) * 180.0 / M_PI;
			double suppDeg  = 180.0 - angleDeg;

			CString strAngle1, strAngle2;
			strAngle1.Format(_T("%.0f\u00B0"), angleDeg);
			strAngle2.Format(_T("%.0f\u00B0"), suppDeg);

			// Draw both angle labels near intersection
			pDC->SelectObject(&pen);
			CPoint pi = ct.ToLogical(ix, iy);
			int oldBk = pDC->SetBkMode(TRANSPARENT);
			pDC->TextOut(pi.x + 5,  pi.y - 16, strAngle1);
			pDC->TextOut(pi.x - 35, pi.y - 16, strAngle2);
			pDC->SetBkMode(oldBk);

			// If parallel line exists, draw intersection there too
			if (seg.bParallel && len > 1e-10)
			{
				double nx = -dy / len, ny = dx / len;
				double off = seg.dParallelOffset;
				// Parallel line: P1+nx*off + s*(dx,dy)
				double px1 = seg.x1 + nx*off, py1 = seg.y1 + ny*off;
				double denom2 = dx * sinA - dy * cosA;
				if (std::abs(denom2) > 1e-10)
				{
					double s2 = ((tx - px1)*sinA - (ty - py1)*cosA) / denom2;
					double ix2 = px1 + s2*dx;
					double iy2 = py1 + s2*dy;
					CPoint pi2 = ct.ToLogical(ix2, iy2);
					pDC->TextOut(pi2.x + 5,  pi2.y - 16, strAngle1);
					pDC->TextOut(pi2.x - 35, pi2.y - 16, strAngle2);
				}
			}
		}
	}

	pDC->SelectObject(pOld);
}

// ---------------------------------------------------------------------------
// DrawGeoTriangle — triangle with labels, angle arcs, and optional circles
// ---------------------------------------------------------------------------
static void DrawAngleArc(CDC* pDC, const CoordTransform& ct,
                         double vx, double vy,          // vertex in math coords
                         double d1x, double d1y,        // direction toward first adjacent vertex
                         double d2x, double d2y,        // direction toward second adjacent vertex
                         double radiusMath,             // arc radius in math units
                         bool bYDown)                   // true=screen (Y down), false=print (Y up)
{
	// Compute arc start/end angles in screen-coordinate space
	double len1 = std::sqrt(d1x*d1x + d1y*d1y);
	double len2 = std::sqrt(d2x*d2x + d2y*d2y);
	if (len1 < 1e-10 || len2 < 1e-10) return;
	d1x /= len1; d1y /= len1;
	d2x /= len2; d2y /= len2;

	CPoint pV = ct.ToLogical(vx, vy);
	CPoint pE1 = ct.ToLogical(vx + d1x * radiusMath, vy + d1y * radiusMath);
	CPoint pE2 = ct.ToLogical(vx + d2x * radiusMath, vy + d2y * radiusMath);

	// Compute bounding box of the arc circle in screen pixels
	// The radius in pixels: use the scale of the transform
	double scX = ct.ScaleX();
	double scY = ct.ScaleY();
	int rPx = (int)(radiusMath * (scX + scY) * 0.5);
	if (rPx < 3) return;

	// Choose arc direction so it sweeps through the interior of the angle.
	// cross = d1 × d2 in math (Y-up) coords.
	// CDC::Arc draws CCW in logical coordinates.
	//   bYDown (MM_TEXT, Y-down): CCW logical = CW visually.
	//     Arc(pE1,pE2) sweeps CW visually; this is the interior arc when cross > 0.
	//     When cross < 0, swap endpoints so we still get the interior arc.
	//   !bYDown (MM_LOMETRIC, Y-up): CCW logical = CCW visually.
	//     Arc(pE2,pE1) sweeps CW visually; interior arc when cross > 0.
	//     When cross < 0, swap back to Arc(pE1,pE2) for the interior arc.
	double cross = d1x * d2y - d1y * d2x;
	if (std::abs(cross) < 1e-10) return;  // degenerate: directions are (anti-)parallel

	CRect rcArc(pV.x - rPx, pV.y - rPx, pV.x + rPx, pV.y + rPx);
	bool doSwap = bYDown ? (cross < 0) : (cross > 0);
	if (doSwap)
		pDC->Arc(rcArc, pE2, pE1);
	else
		pDC->Arc(rcArc, pE1, pE2);
}

static void DrawGeoTriangle(CDC* pDC, const CoordTransform& ct, const GeoTriangle& tri)
{
	// --- Draw the three sides ---
	CPen pen(PS_SOLID, 2, tri.color);
	CPen* pOld = pDC->SelectObject(&pen);

	CPoint pA = ct.ToLogical(tri.ax, tri.ay);
	CPoint pB = ct.ToLogical(tri.bx, tri.by);
	CPoint pC = ct.ToLogical(tri.cx, tri.cy);

	pDC->MoveTo(pA); pDC->LineTo(pB);
	pDC->MoveTo(pB); pDC->LineTo(pC);
	pDC->MoveTo(pC); pDC->LineTo(pA);

	// Small circles at vertices
	for (const auto& p : {pA, pB, pC})
		pDC->Ellipse(p.x-3, p.y-3, p.x+3, p.y+3);

	int oldBk = pDC->SetBkMode(TRANSPARENT);

	// --- Vertex labels ---
	if (tri.bShowVertexLabels)
	{
		// Offset label away from opposite side
		auto vertexOffset = [&](double vx, double vy, double ox, double oy) -> CSize {
			// Compute outward direction from centroid
			double cg_x = (tri.ax + tri.bx + tri.cx) / 3.0;
			double cg_y = (tri.ay + tri.by + tri.cy) / 3.0;
			double dirX = vx - cg_x, dirY = vy - cg_y;
			double norm = std::sqrt(dirX*dirX + dirY*dirY);
			if (norm < 1e-10) return CSize(4, -14);
			dirX /= norm; dirY /= norm;
			// In screen Y-down: flip Y
			int ox2 = (int)(dirX * 16.0);
			int oy2 = ct.bYDown ? (int)(dirY * 16.0) : (int)(-dirY * 16.0);
			return CSize(ox2, oy2 - 8);
		};
		auto szA = vertexOffset(tri.ax, tri.ay, 0, 0);
		auto szB = vertexOffset(tri.bx, tri.by, 0, 0);
		auto szC = vertexOffset(tri.cx, tri.cy, 0, 0);
		pDC->TextOut(pA.x + szA.cx, pA.y + szA.cy, tri.labelA);
		pDC->TextOut(pB.x + szB.cx, pB.y + szB.cy, tri.labelB);
		pDC->TextOut(pC.x + szC.cx, pC.y + szC.cy, tri.labelC);
	}

	// --- Side labels (midpoint of each side) ---
	if (tri.bShowSideLabels)
	{
		// Side a = BC (opposite A)
		double mx_a = (tri.bx + tri.cx) * 0.5, my_a = (tri.by + tri.cy) * 0.5;
		// Side b = AC (opposite B)
		double mx_b = (tri.ax + tri.cx) * 0.5, my_b = (tri.ay + tri.cy) * 0.5;
		// Side c = AB (opposite C)
		double mx_c = (tri.ax + tri.bx) * 0.5, my_c = (tri.ay + tri.by) * 0.5;

		DrawLabel(pDC, ct, mx_a, my_a, tri.labelSideA, 4, -14);
		DrawLabel(pDC, ct, mx_b, my_b, tri.labelSideB, 4, -14);
		DrawLabel(pDC, ct, mx_c, my_c, tri.labelSideC, 4, -14);
	}

	// --- Angle labels and arcs ---
	// Estimate a reasonable arc radius = 15% of the shortest side
	double a = tri.SideA(), b = tri.SideB(), c = tri.SideC();
	double minSide = std::min({a, b, c});
	double arcR = minSide * 0.20;
	if (arcR < 0.1) arcR = 0.1;

	if (tri.bShowAngleLabels || tri.bShowAngleValues)
	{
		auto drawAngleLabel = [&](double vx, double vy,
		                          double nb1x, double nb1y,
		                          double nb2x, double nb2y,
		                          const CString& lbl, double angleDeg)
		{
			// Draw arc
			{
				CPen thinPen(PS_SOLID, 1, tri.color);
				pDC->SelectObject(&thinPen);
				DrawAngleArc(pDC, ct, vx, vy,
				             nb1x-vx, nb1y-vy, nb2x-vx, nb2y-vy,
				             arcR, ct.bYDown);
				pDC->SelectObject(&pen);
			}

			// Bisector direction for label placement
			double d1x = nb1x-vx, d1y = nb1y-vy;
			double d2x = nb2x-vx, d2y = nb2y-vy;
			double len1 = std::sqrt(d1x*d1x + d1y*d1y);
			double len2 = std::sqrt(d2x*d2x + d2y*d2y);
			if (len1 < 1e-10 || len2 < 1e-10) return;
			double bx_ = d1x/len1 + d2x/len2;
			double by_ = d1y/len1 + d2y/len2;
			double blen = std::sqrt(bx_*bx_ + by_*by_);
			if (blen < 1e-10) return;
			bx_ /= blen; by_ /= blen;

			double labelX = vx + bx_ * arcR * 1.6;
			double labelY = vy + by_ * arcR * 1.6;

			CString text = lbl;
			if (tri.bShowAngleValues)
			{
				CString val;
				val.Format(_T("=%.0f\u00B0"), angleDeg);
				text += val;
			}
			DrawLabel(pDC, ct, labelX, labelY, text, -6, -8);
		};

		drawAngleLabel(tri.ax, tri.ay, tri.bx, tri.by, tri.cx, tri.cy,
		               tri.labelAngleA, tri.AngleAtA());
		drawAngleLabel(tri.bx, tri.by, tri.ax, tri.ay, tri.cx, tri.cy,
		               tri.labelAngleB, tri.AngleAtB());
		drawAngleLabel(tri.cx, tri.cy, tri.ax, tri.ay, tri.bx, tri.by,
		               tri.labelAngleC, tri.AngleAtC());
	}

	// --- Right angle mark at C if type is TT_RIGHT_C ---
	if (tri.type == TT_RIGHT_C)
	{
		// Draw a small square at vertex C
		double caX = tri.ax - tri.cx, caY = tri.ay - tri.cy;
		double cbX = tri.bx - tri.cx, cbY = tri.by - tri.cy;
		double lenCA = std::sqrt(caX*caX + caY*caY);
		double lenCB = std::sqrt(cbX*cbX + cbY*cbY);
		if (lenCA > 1e-10 && lenCB > 1e-10)
		{
			double sq = minSide * 0.07;
			double uaX = caX/lenCA * sq, uaY = caY/lenCA * sq;
			double ubX = cbX/lenCB * sq, ubY = cbY/lenCB * sq;
			// Four corners of the right-angle square
			CPoint sq1 = ct.ToLogical(tri.cx + uaX,           tri.cy + uaY);
			CPoint sq2 = ct.ToLogical(tri.cx + uaX + ubX,     tri.cy + uaY + ubY);
			CPoint sq3 = ct.ToLogical(tri.cx + ubX,           tri.cy + ubY);

			CPen thinPen(PS_SOLID, 1, tri.color);
			pDC->SelectObject(&thinPen);
			pDC->MoveTo(sq1); pDC->LineTo(sq2); pDC->LineTo(sq3);
			pDC->SelectObject(&pen);
		}
	}

	// --- Circumcircle ---
	if (tri.bCircumcircle)
	{
		double ox, oy, r;
		tri.GetCircumcircle(ox, oy, r);
		if (r > 1e-10)
		{
			CPen circPen(PS_DASH, 1, RGB(180, 100, 20));
			pDC->SelectObject(&circPen);
			CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
			CPoint pcen = ct.ToLogical(ox, oy);
			double scX = ct.ScaleX(), scY = ct.ScaleY();
			int rpx  = (int)(r * scX);
			int rpxy = (int)(r * scY);
			pDC->Ellipse(pcen.x - rpx, pcen.y - rpxy, pcen.x + rpx, pcen.y + rpxy);
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(&pen);
		}
	}

	// --- Incircle ---
	if (tri.bIncircle)
	{
		double ix, iy, r;
		tri.GetIncircle(ix, iy, r);
		if (r > 1e-10)
		{
			CPen incPen(PS_DOT, 1, RGB(20, 100, 200));
			pDC->SelectObject(&incPen);
			CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
			CPoint pcen = ct.ToLogical(ix, iy);
			double scX = ct.ScaleX(), scY = ct.ScaleY();
			int rpx  = (int)(r * scX);
			int rpxy = (int)(r * scY);
			pDC->Ellipse(pcen.x - rpx, pcen.y - rpxy, pcen.x + rpx, pcen.y + rpxy);
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(&pen);
		}
	}

	// --- Perpendicular bisectors (keskinormaalit) ---
	if (tri.bPerpBisectors)
	{
		CPen perpPen(PS_DASH, 1, RGB(140, 40, 200));
		pDC->SelectObject(&perpPen);
		double ext = std::max({a, b, c}) * 0.4;

		auto drawPerpBisector = [&](double x1, double y1, double x2, double y2)
		{
			double mx = (x1 + x2) * 0.5, my = (y1 + y2) * 0.5;
			double dx = x2 - x1, dy = y2 - y1;
			double len = std::sqrt(dx*dx + dy*dy);
			if (len < 1e-10) return;
			double nx = -dy / len, ny = dx / len;
			CPoint p1 = ct.ToLogical(mx + nx * ext, my + ny * ext);
			CPoint p2 = ct.ToLogical(mx - nx * ext, my - ny * ext);
			pDC->MoveTo(p1); pDC->LineTo(p2);
		};
		drawPerpBisector(tri.bx, tri.by, tri.cx, tri.cy);
		drawPerpBisector(tri.ax, tri.ay, tri.cx, tri.cy);
		drawPerpBisector(tri.ax, tri.ay, tri.bx, tri.by);
		pDC->SelectObject(&pen);
	}

	// --- Angle bisectors (kulmien puolittajat) ---
	if (tri.bAngleBisectors)
	{
		CPen bisPen(PS_DASH, 1, RGB(180, 110, 0));
		pDC->SelectObject(&bisPen);
		// Angle bisector theorem: bisector from vertex V divides opposite side
		// in the ratio of the adjacent side lengths.
		// From A: meets BC at D_a = b/(b+c)*B + c/(b+c)*C  (b=|AC|, c=|AB|)
		// From B: meets AC at D_b = a/(a+c)*A + c/(a+c)*C  (a=|BC|, c=|AB|)
		// From C: meets AB at D_c = a/(a+b)*A + b/(a+b)*B  (a=|BC|, b=|AC|)
		double dax = (b * tri.bx + c * tri.cx) / (b + c);
		double day = (b * tri.by + c * tri.cy) / (b + c);
		double dbx = (a * tri.ax + c * tri.cx) / (a + c);
		double dby = (a * tri.ay + c * tri.cy) / (a + c);
		double dcx = (a * tri.ax + b * tri.bx) / (a + b);
		double dcy = (a * tri.ay + b * tri.by) / (a + b);

		CPoint pA2 = ct.ToLogical(tri.ax, tri.ay);
		CPoint pB2 = ct.ToLogical(tri.bx, tri.by);
		CPoint pC2 = ct.ToLogical(tri.cx, tri.cy);
		CPoint pDa = ct.ToLogical(dax, day);
		CPoint pDb = ct.ToLogical(dbx, dby);
		CPoint pDc = ct.ToLogical(dcx, dcy);
		pDC->MoveTo(pA2); pDC->LineTo(pDa);
		pDC->MoveTo(pB2); pDC->LineTo(pDb);
		pDC->MoveTo(pC2); pDC->LineTo(pDc);
		pDC->SelectObject(&pen);
	}

	// --- Altitudes (korkeusjanat) ---
	if (tri.bAltitudes)
	{
		CPen altPen(PS_SOLID, 1, RGB(200, 0, 140));
		pDC->SelectObject(&altPen);
		// Foot of perpendicular from point (vx,vy) onto line (p1)-(p2)
		auto footPt = [](double vx, double vy,
		                 double p1x, double p1y, double p2x, double p2y,
		                 double& hx, double& hy)
		{
			double dx = p2x - p1x, dy = p2y - p1y;
			double len2 = dx*dx + dy*dy;
			if (len2 < 1e-20) { hx = p1x; hy = p1y; return; }
			double t = ((vx - p1x)*dx + (vy - p1y)*dy) / len2;
			hx = p1x + t * dx;
			hy = p1y + t * dy;
		};
		double hax, hay, hbx, hby, hcx, hcy;
		footPt(tri.ax, tri.ay, tri.bx, tri.by, tri.cx, tri.cy, hax, hay);
		footPt(tri.bx, tri.by, tri.ax, tri.ay, tri.cx, tri.cy, hbx, hby);
		footPt(tri.cx, tri.cy, tri.ax, tri.ay, tri.bx, tri.by, hcx, hcy);

		CPoint pA3 = ct.ToLogical(tri.ax, tri.ay);
		CPoint pB3 = ct.ToLogical(tri.bx, tri.by);
		CPoint pC3 = ct.ToLogical(tri.cx, tri.cy);
		CPoint pHa = ct.ToLogical(hax, hay);
		CPoint pHb = ct.ToLogical(hbx, hby);
		CPoint pHc = ct.ToLogical(hcx, hcy);
		pDC->MoveTo(pA3); pDC->LineTo(pHa);
		pDC->MoveTo(pB3); pDC->LineTo(pHb);
		pDC->MoveTo(pC3); pDC->LineTo(pHc);
		pDC->SelectObject(&pen);
	}

	// --- Medians (keskijanat / mediaanit) ---
	if (tri.bMedians)
	{
		CPen medPen(PS_SOLID, 1, RGB(0, 150, 210));
		pDC->SelectObject(&medPen);
		// Each median goes from a vertex to the midpoint of the opposite side
		CPoint pA4 = ct.ToLogical(tri.ax, tri.ay);
		CPoint pB4 = ct.ToLogical(tri.bx, tri.by);
		CPoint pC4 = ct.ToLogical(tri.cx, tri.cy);
		CPoint pMa = ct.ToLogical((tri.bx + tri.cx) * 0.5, (tri.by + tri.cy) * 0.5);
		CPoint pMb = ct.ToLogical((tri.ax + tri.cx) * 0.5, (tri.ay + tri.cy) * 0.5);
		CPoint pMc = ct.ToLogical((tri.ax + tri.bx) * 0.5, (tri.ay + tri.by) * 0.5);
		pDC->MoveTo(pA4); pDC->LineTo(pMa);
		pDC->MoveTo(pB4); pDC->LineTo(pMb);
		pDC->MoveTo(pC4); pDC->LineTo(pMc);
		pDC->SelectObject(&pen);
	}

	pDC->SetBkMode(oldBk);
	pDC->SelectObject(pOld);
}
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

void CGraphDrawerDoc::ReplaceUserCurve(int idx, const UserCurve& curveDef)
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
		if (idx >= 0 && idx < (int)m_vecUserCurves.size())
			m_vecUserCurves[idx] = std::move(c);
	}
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
// Geometric objects
// ---------------------------------------------------------------------------
void CGraphDrawerDoc::AddGeoObject(const GeoObject& obj)
{
	{
		CSingleLock lock(&m_csGeoObjects, TRUE);
		m_vecGeoObjects.push_back(obj);
	}
	SetModifiedFlag();
	UpdateAllViews(NULL);
}

void CGraphDrawerDoc::ReplaceGeoObject(int idx, const GeoObject& obj)
{
	{
		CSingleLock lock(&m_csGeoObjects, TRUE);
		if (idx < 0 || idx >= (int)m_vecGeoObjects.size()) return;
		m_vecGeoObjects[idx] = obj;
	}
	SetModifiedFlag();
	UpdateAllViews(NULL);
}

void CGraphDrawerDoc::RemoveGeoObject(int idx)
{
	{
		CSingleLock lock(&m_csGeoObjects, TRUE);
		if (idx < 0 || idx >= (int)m_vecGeoObjects.size()) return;
		m_vecGeoObjects.erase(m_vecGeoObjects.begin() + idx);
	}
	SetModifiedFlag();
	UpdateAllViews(NULL);
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

