#pragma once
#include <string>
#include <vector>

// Recursive-descent expression parser for y = f(x).
// Supported operators:  + - * / ^ (power), unary minus
// Supported functions:  sin, cos, tan, asin, acos, atan,
//                       sinh, cosh, tanh, exp, log/ln,
//                       log10, sqrt, abs, sign, floor, ceil, round
// Constants:  pi, e
// Variable:   x
//
// Usage:
//   CExpressionParser p;
//   if (p.Parse(_T("x*sin(x) + cos(x)"))) {
//       double y;
//       if (p.Evaluate(1.5, y)) { /* use y */ }
//   }

class CExpressionParser
{
public:
    CExpressionParser();
    ~CExpressionParser();

    // Parse expression string. Returns true on syntactic success.
    bool    Parse(const CString& strExpr);

    // Evaluate for a given x. Returns false on domain/math error.
    bool    Evaluate(double x, double& result) const;

    // Evaluate for a given t (parametric curves). Identical to Evaluate but
    // the expression may use 't' (or 'x') as the free variable.
    bool    EvaluateT(double t, double& result) const;

    // Evaluate for given x and y (for implicit curves)
    bool    Evaluate(double x, double y, double& result) const;

    // Error description (valid after Parse returns false).
    CString GetError() const { return m_strError; }

private:
    enum TokenType {
        TOK_NUMBER, TOK_IDENT,
        TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_POW,
        TOK_LPAREN, TOK_RPAREN,
        TOK_END, TOK_ERROR
    };

    struct Token {
        TokenType    type;
        double       numVal;
        std::wstring strVal;
        explicit Token(TokenType t)                     : type(t), numVal(0.0) {}
        explicit Token(double v)                        : type(TOK_NUMBER), numVal(v) {}
        Token(TokenType t, const std::wstring& s)       : type(t), numVal(0.0), strVal(s) {}
    };

    std::vector<Token>  m_tokens;
    mutable size_t      m_pos;
    mutable double      m_x;
    mutable double      m_y;
    mutable CString     m_strError;

    bool   Tokenize(const std::wstring& expr);

    // Recursive-descent levels
    double ParseExpr()    const; // +  -
    double ParseTerm()    const; // *  /
    double ParsePower()   const; // ^
    double ParseUnary()   const; // unary -
    double ParsePrimary() const; // number | x | pi | e | func(expr) | (expr)

    double CallFunction(const std::wstring& name, double arg) const;

    const Token& Current() const;
    void         Advance() const;
};
