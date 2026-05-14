#pragma once
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------------
// GeometricObjects.h — data structures for geometric figures
// ---------------------------------------------------------------------------

enum GeoObjectType { GOT_LINE_SEGMENT, GOT_TRIANGLE };

// ---------------------------------------------------------------------------
// GeoLineSegment — line segment with optional parallel and transversal lines
// ---------------------------------------------------------------------------
struct GeoLineSegment
{
    double   x1, y1;              // start point
    double   x2, y2;              // end point
    CString  label;
    COLORREF color;
    BOOL     bVisible;

    // Parallel line
    BOOL     bParallel;
    double   dParallelOffset;     // perpendicular distance

    // Transversal (intersecting) line
    BOOL     bTransversal;
    double   dTransversalX;       // a point on the transversal
    double   dTransversalY;
    double   dTransversalAngleDeg; // angle in degrees relative to X-axis

    GeoLineSegment()
        : x1(-3.0), y1(0.0), x2(3.0), y2(0.0)
        , color(RGB(0, 120, 220)), bVisible(TRUE)
        , bParallel(FALSE), dParallelOffset(2.0)
        , bTransversal(FALSE)
        , dTransversalX(0.0), dTransversalY(0.0)
        , dTransversalAngleDeg(60.0)
    {}
};

// ---------------------------------------------------------------------------
// GeoTriangle — triangle with labels, angle arcs, and optional circles
// ---------------------------------------------------------------------------
enum TriangleType { TT_GENERAL = 0, TT_RIGHT_C = 1, TT_ISOSCELES = 2, TT_EQUILATERAL = 3 };

struct GeoTriangle
{
    double   ax, ay;    // Vertex A
    double   bx, by;    // Vertex B
    double   cx, cy;    // Vertex C

    // Labels
    CString  labelA, labelB, labelC;
    CString  labelSideA, labelSideB, labelSideC;  // side a is opposite to A, etc.
    CString  labelAngleA, labelAngleB, labelAngleC;

    // Display options
    BOOL     bShowVertexLabels;
    BOOL     bShowSideLabels;
    BOOL     bShowAngleLabels;
    BOOL     bShowAngleValues;    // show numeric degree values

    // Circles
    BOOL     bCircumcircle;
    BOOL     bIncircle;

    // Lines
    BOOL     bPerpBisectors;   // sivujen keskinormaalit
    BOOL     bAngleBisectors;  // kulmien puolittajat
    BOOL     bAltitudes;       // korkeusjanat
    BOOL     bMedians;         // keskijanat / mediaanit

    TriangleType type;
    CString  label;
    COLORREF color;
    BOOL     bVisible;

    GeoTriangle()
        : ax(0.0), ay(0.0), bx(4.0), by(0.0), cx(2.0), cy(3.0)
        , labelA(_T("A")), labelB(_T("B")), labelC(_T("C"))
        , labelSideA(_T("a")), labelSideB(_T("b")), labelSideC(_T("c"))
        , labelAngleA(_T("\u03B1")), labelAngleB(_T("\u03B2")), labelAngleC(_T("\u03B3"))
        , bShowVertexLabels(TRUE), bShowSideLabels(TRUE)
        , bShowAngleLabels(TRUE), bShowAngleValues(FALSE)
        , bCircumcircle(FALSE), bIncircle(FALSE)
        , bPerpBisectors(FALSE), bAngleBisectors(FALSE)
        , bAltitudes(FALSE), bMedians(FALSE)
        , type(TT_GENERAL), color(RGB(0, 160, 80)), bVisible(TRUE)
    {}

    // Helper: compute signed area (positive if CCW)
    double SignedArea() const
    {
        return 0.5 * ((bx - ax) * (cy - ay) - (cx - ax) * (by - ay));
    }
    double Area() const { return std::abs(SignedArea()); }

    // Side lengths
    double SideA() const { return std::sqrt((cx-bx)*(cx-bx)+(cy-by)*(cy-by)); } // opposite A
    double SideB() const { return std::sqrt((cx-ax)*(cx-ax)+(cy-ay)*(cy-ay)); } // opposite B
    double SideC() const { return std::sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)); } // opposite C

    // Angles in degrees at each vertex
    double AngleAtA() const
    {
        double ab_x = bx-ax, ab_y = by-ay;
        double ac_x = cx-ax, ac_y = cy-ay;
        double lenAB = std::sqrt(ab_x*ab_x + ab_y*ab_y);
        double lenAC = std::sqrt(ac_x*ac_x + ac_y*ac_y);
        if (lenAB < 1e-12 || lenAC < 1e-12) return 0.0;
        double cosA = (ab_x*ac_x + ab_y*ac_y) / (lenAB * lenAC);
        cosA = std::max(-1.0, std::min(1.0, cosA));
        return std::acos(cosA) * 180.0 / M_PI;
    }
    double AngleAtB() const
    {
        double ba_x = ax-bx, ba_y = ay-by;
        double bc_x = cx-bx, bc_y = cy-by;
        double lenBA = std::sqrt(ba_x*ba_x + ba_y*ba_y);
        double lenBC = std::sqrt(bc_x*bc_x + bc_y*bc_y);
        if (lenBA < 1e-12 || lenBC < 1e-12) return 0.0;
        double cosB = (ba_x*bc_x + ba_y*bc_y) / (lenBA * lenBC);
        cosB = std::max(-1.0, std::min(1.0, cosB));
        return std::acos(cosB) * 180.0 / M_PI;
    }
    double AngleAtC() const
    {
        double ca_x = ax-cx, ca_y = ay-cy;
        double cb_x = bx-cx, cb_y = by-cy;
        double lenCA = std::sqrt(ca_x*ca_x + ca_y*ca_y);
        double lenCB = std::sqrt(cb_x*cb_x + cb_y*cb_y);
        if (lenCA < 1e-12 || lenCB < 1e-12) return 0.0;
        double cosC = (ca_x*cb_x + ca_y*cb_y) / (lenCA * lenCB);
        cosC = std::max(-1.0, std::min(1.0, cosC));
        return std::acos(cosC) * 180.0 / M_PI;
    }

    // Circumcenter (cx_circ, cy_circ) and circumradius
    void GetCircumcircle(double& ox, double& oy, double& r) const
    {
        double D = 2.0 * (ax*(by-cy) + bx*(cy-ay) + cx*(ay-by));
        if (std::abs(D) < 1e-12) { ox=oy=r=0.0; return; }
        ox = ((ax*ax+ay*ay)*(by-cy) + (bx*bx+by*by)*(cy-ay) + (cx*cx+cy*cy)*(ay-by)) / D;
        oy = ((ax*ax+ay*ay)*(cx-bx) + (bx*bx+by*by)*(ax-cx) + (cx*cx+cy*cy)*(bx-ax)) / D;
        r = std::sqrt((ox-ax)*(ox-ax) + (oy-ay)*(oy-ay));
    }

    // Incenter and inradius
    void GetIncircle(double& ix, double& iy, double& r) const
    {
        double a = SideA(), b = SideB(), c = SideC();
        double perim = a + b + c;
        if (perim < 1e-12) { ix=iy=r=0.0; return; }
        ix = (a*ax + b*bx + c*cx) / perim;
        iy = (a*ay + b*by + c*cy) / perim;
        double area = Area();
        r = (perim > 1e-12) ? (2.0 * area / perim) : 0.0;
    }
};

// ---------------------------------------------------------------------------
// GeoObject — a variant holding one geometric object
// ---------------------------------------------------------------------------
struct GeoObject
{
    GeoObjectType type;
    GeoLineSegment line;
    GeoTriangle    triangle;

    GeoObject() : type(GOT_LINE_SEGMENT) {}

    CString GetDisplayName() const
    {
        if (type == GOT_LINE_SEGMENT)
            return line.label.IsEmpty() ? CString(_T("Line Segment")) : line.label;
        else
            return triangle.label.IsEmpty() ? CString(_T("Triangle")) : triangle.label;
    }

    BOOL IsVisible() const
    {
        return (type == GOT_LINE_SEGMENT) ? line.bVisible : triangle.bVisible;
    }
    void SetVisible(BOOL b)
    {
        if (type == GOT_LINE_SEGMENT) line.bVisible = b;
        else triangle.bVisible = b;
    }
};
