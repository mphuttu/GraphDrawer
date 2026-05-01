// GraphDrawerDoc.h : interface of the CGraphDrawerDoc class
//
#include "DrawOptionsDialog.h"
#include "ExpressionParser.h"
#include <vector>
#include <utility>
#include <cmath>

#pragma once
struct DrawOptionsData;

// ---------------------------------------------------------------------------
// CoordTransform — maps math coordinates to MM_LOMETRIC logical coordinates.
//
//  Top-left of client  = logical (0, 0)           = math (xMin, yMax)
//  Bottom-right        = logical (clientW, -clientH) = math (xMax, yMin)
//
//  Usage:
//      CoordTransform ct(xMin, xMax, yMin, yMax, clientW, clientH);
//      CPoint p = ct.ToLogical(mathX, mathY);
// ---------------------------------------------------------------------------
struct CoordTransform
{
    double xMin, xMax;
    double yMin, yMax;
    int    clientW;   // abs logical/pixel width  (positive)
    int    clientH;   // abs logical/pixel height (positive)
    bool   bLogX;     // true → logarithmic X axis
    bool   bLogY;     // true → logarithmic Y axis
    bool   bYDown;    // true = Y increases downward (pixel/MM_TEXT), false = MM_LOMETRIC (Y up)

    CoordTransform()
        : xMin(-10), xMax(10), yMin(-10), yMax(10), clientW(1), clientH(1)
        , bLogX(false), bLogY(false), bYDown(false) {}

    CoordTransform(double xmn, double xmx, double ymn, double ymx, int w, int h,
                   bool logX = false, bool logY = false, bool yDown = false)
        : xMin(xmn), xMax(xmx), yMin(ymn), yMax(ymx), clientW(w), clientH(h)
        , bLogX(logX), bLogY(logY), bYDown(yDown) {}

    CPoint ToLogical(double mx, double my) const
    {
        double fx, fy;

        // --- X ---
        if (bLogX)
        {
            if (mx <= 0.0 || xMin <= 0.0 || xMax <= 0.0) { fx = -1e7; }
            else
            {
                double lx    = std::log10(mx);
                double lxMin = std::log10(xMin);
                double lxMax = std::log10(xMax);
                fx = (lxMax > lxMin) ? (lx - lxMin) / (lxMax - lxMin) * clientW : -1e7;
            }
        }
        else
        {
            fx = (xMax > xMin) ? (mx - xMin) / (xMax - xMin) * clientW : -1e7;
        }

        // --- Y ---
        if (bLogY)
        {
            if (my <= 0.0 || yMin <= 0.0 || yMax <= 0.0)
            {
                fy = bYDown ? 1e7 : -1e7;
            }
            else
            {
                double ly    = std::log10(my);
                double lyMin = std::log10(yMin);
                double lyMax = std::log10(yMax);
                if (lyMax <= lyMin)
                    fy = bYDown ? 1e7 : -1e7;
                else if (bYDown)
                    fy =  ((lyMax - ly) / (lyMax - lyMin)) * clientH;  // 0=top, +H=bottom
                else
                    fy = -((lyMax - ly) / (lyMax - lyMin)) * clientH;  // 0=top, -H=bottom (MM_LOMETRIC)
            }
        }
        else
        {
            if (yMax <= yMin)
                fy = bYDown ? 1e7 : -1e7;
            else if (bYDown)
                fy =  (yMax - my) / (yMax - yMin) * clientH;   // 0 at top (y=yMax), +H at bottom
            else
                fy = -(yMax - my) / (yMax - yMin) * clientH;   // 0 at top,  -H at bottom (MM_LOMETRIC)
        }

        // Clamp to prevent undefined-behaviour int overflow for far-off-screen points
        // (NaN also handled: !(NaN > -1e7) is true → clamped to -1e7).
        if (!(fx > -1e7)) fx = -1e7; else if (fx > 1e7) fx = 1e7;
        if (!(fy > -1e7)) fy = -1e7; else if (fy > 1e7) fy = 1e7;
        return CPoint(static_cast<int>(fx), static_cast<int>(fy));
    }

    // Pixels (logical units) per math unit along X / Y
    double ScaleX() const
    {
        if (bLogX && xMin > 0 && xMax > xMin)
            return (double)clientW / (std::log10(xMax) - std::log10(xMin));
        return (xMax > xMin) ? (double)clientW / (xMax - xMin) : 1.0;
    }
    double ScaleY() const
    {
        if (bLogY && yMin > 0 && yMax > yMin)
            return (double)clientH / (std::log10(yMax) - std::log10(yMin));
        return (yMax > yMin) ? (double)clientH / (yMax - yMin) : 1.0;
    }
};

// ---------------------------------------------------------------------------
// UserCurve — a user-added curve (y=f(x) or parametric x(t), y(t))
// ---------------------------------------------------------------------------
enum UserCurveType { UCT_YFX, UCT_PARAMETRIC, UCT_POLAR, UCT_IMPLICIT };

struct UserCurve
{
    UserCurveType type;
    CString       label;
    BOOL          bVisible;
    COLORREF      color;

    // y = f(x) mode  (variable name 'x')
    CString strExprY;
    double  xStart, xEnd;

    // Parametric mode  (variable name 't')
    CString strExprX;
    CString strExprYPar;
    double  tStart, tEnd;

    // Polar mode  (variable name 'phi')
    CString strExprPolar;
    double  phiStart, phiEnd;

    // Implicit mode  (variable name 'x' or 'y')
    CString strExprImplicit;
    double  xStartImp = -2.0, xEndImp = 2.0;
    double  yStartImp = -2.0, yEndImp = 2.0;

	struct MathPoint { double x, y; };
	std::vector<MathPoint> points; // Käytössä y=f(x), param, polar
	std::vector<std::pair<MathPoint, MathPoint>> segments; // Käytössä implicit

    UserCurve()
        : type(UCT_YFX), bVisible(TRUE), color(RGB(255, 200, 0))
        , xStart(-10.0), xEnd(10.0)
        , tStart(0.0),   tEnd(6.283185307179586)
        , phiStart(0.0), phiEnd(6.283185307179586) {}
};
	
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

	// -----------------------------------------------------------------------
	// Custom expression  y = f(x)
	// -----------------------------------------------------------------------
	BOOL    m_bDrawCustomFunction;
	CString m_strCustomExpression;
	double  m_dCustomRangeStart;   // x range [a, b] used when plotting
	double  m_dCustomRangeEnd;

	// Parsed math-coordinate points produced by the worker thread.
	// Protected by m_csCustomPoints.
	struct MathPoint { double x, y; };
	std::vector<MathPoint>  m_vecCustomPoints;
	CCriticalSection        m_csCustomPoints;

	// Call this from the dialog when the expression or the enable-flag changes.
	// Starts (or restarts) the background computation thread.
	void SetCustomExpression(const CString& expr, BOOL bDraw,
	                         double xStart = -20.0, double xEnd = 20.0);

	// -----------------------------------------------------------------------
	// User-defined curves (multiple, with optional visibility)
	// -----------------------------------------------------------------------
	std::vector<UserCurve>  m_vecUserCurves;
	CCriticalSection        m_csUserCurves;

	// Add a new curve (computes points synchronously, then invalidates views).
	void AddUserCurve(const UserCurve& curve);

	// Remove the curve at the given index.
	void RemoveUserCurve(int idx);

	// Toggle visibility flag.
	void SetUserCurveVisible(int idx, BOOL bVisible);

	// Recompute all user-curve points (call after range change).
	void RecomputeUserCurves();

private:
	// Worker thread --------------------------------------------------------
	volatile BOOL  m_bCancelDraw;   // set to TRUE to ask the thread to stop
	CWinThread*    m_pDrawThread;   // NULL when idle

	struct ThreadParams
	{
		CGraphDrawerDoc* pDoc;
		CString          strExpr;
		double           xStart;
		double           xEnd;
		double           xStep;
		bool             bLogX;  // true → näytteenotto log-avaruudessa
	};

	static UINT DrawThreadProc(LPVOID pParam);
};

void DrawCoordinateAxes(CDC* pDC, BOOL bShowTicks, int nTicksInterval,
						BOOL bShowLabels, const CoordTransform& ct);

void DrawSine( CDC* pDC, const CoordTransform& ct, COLORREF clrSineColor);

void DrawCosine( CDC* pDC, const CoordTransform& ct, COLORREF clrCosineColor);

void DrawTan( CDC* pDC, const CoordTransform& ct, COLORREF clrTanColor);

void DrawCotan( CDC* pDC, const CoordTransform& ct, COLORREF clrCotanColor);

void DrawExp( CDC* pDC, const CoordTransform& ct, COLORREF clrExpColor);

void DrawLN( CDC* pDC, const CoordTransform& ct, COLORREF clrLNColor);

void DrawArcsine( CDC* pDC, const CoordTransform& ct, COLORREF clrArcsineColor);

void DrawArccosine( CDC* pDC, const CoordTransform& ct, COLORREF clrArccosineColor);

void DrawArctan ( CDC* pDC, const CoordTransform& ct, COLORREF clrArctanColor);

double cotan( double x);

double arccotan( double y);

void DrawArccotan ( CDC* pDC, const CoordTransform& ct, COLORREF clrArccotanColor);

void DrawHyperbolicSine ( CDC* pDC, const CoordTransform& ct, COLORREF clrHyperbolicSine);

void DrawHyperbolicCosine( CDC* pDC, const CoordTransform& ct, COLORREF clrHyperbolicCosine);

void DrawHyperbolicTan( CDC* pDC, const CoordTransform& ct, COLORREF clrHyperbolicTan);

double coth(double x);

void DrawHyperbolicCotan( CDC* pDC, const CoordTransform& ct, COLORREF clrHyperbolicCotan);

void DrawCustomFunction(CDC* pDC, const CoordTransform& ct, COLORREF clrColor,
                        CGraphDrawerDoc* pDoc);

void DrawUserCurves(CDC* pDC, const CoordTransform& ct,
                    std::vector<UserCurve>& vecCurves, CCriticalSection& cs);

void DrawCustomFunction( CDC* pDC, int nTicksInterval, CRect m_rcPrintRect,
                         COLORREF clrColor, CGraphDrawerDoc* pDoc);
