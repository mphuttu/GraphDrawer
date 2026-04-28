// ExpressionParser.cpp : Recursive-descent expression parser
//

#include "stdafx.h"
#include "ExpressionParser.h"
#include <cwctype>
#include <cmath>
#include <stdexcept>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Thrown only for structural/syntax problems (unknown token, missing paren, …).
// Domain errors (log of negative number, etc.) use std::runtime_error directly
// so that Parse() can ignore them during its trial evaluation.
struct SyntaxError : public std::runtime_error {
    explicit SyntaxError(const char* msg) : std::runtime_error(msg) {}
};

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
CExpressionParser::CExpressionParser()
    : m_pos(0), m_x(0.0)
{
}

CExpressionParser::~CExpressionParser()
{
}

// ---------------------------------------------------------------------------
// Tokenizer
// ---------------------------------------------------------------------------
bool CExpressionParser::Tokenize(const std::wstring& expr)
{
    m_tokens.clear();
    size_t i = 0;
    const size_t len = expr.size();

    while (i < len)
    {
        wchar_t c = expr[i];

        // Whitespace
        if (iswspace(c)) { ++i; continue; }

        // Number literal (may include a decimal point)
        if (iswdigit(c) || c == L'.')
        {
            size_t start = i;
            bool hasDot = false;
            while (i < len && (iswdigit(expr[i]) || (!hasDot && expr[i] == L'.')))
            {
                if (expr[i] == L'.') hasDot = true;
                ++i;
            }
            // Optional exponent: e+3, e-3, e3 — but only if followed by digit/sign+digit
            if (i < len && (expr[i] == L'e' || expr[i] == L'E'))
            {
                size_t saved = i;
                ++i;
                if (i < len && (expr[i] == L'+' || expr[i] == L'-')) ++i;
                if (i < len && iswdigit(expr[i]))
                {
                    while (i < len && iswdigit(expr[i])) ++i;
                }
                else
                {
                    i = saved; // roll back; the 'e' will be parsed as identifier
                }
            }
            double val = _wtof(expr.substr(start, i - start).c_str());
            m_tokens.push_back(Token(val));
            continue;
        }

        // Identifier / keyword (a-z, A-Z, _, followed by alphanumeric / _)
        if (iswalpha(c) || c == L'_')
        {
            size_t start = i;
            while (i < len && (iswalnum(expr[i]) || expr[i] == L'_')) ++i;
            std::wstring id = expr.substr(start, i - start);
            for (wchar_t& ch : id) ch = towlower(ch);
            m_tokens.push_back(Token(TOK_IDENT, id));
            continue;
        }

        // Single-character tokens
        switch (c)
        {
            case L'+': m_tokens.push_back(Token(TOK_PLUS));   break;
            case L'-': m_tokens.push_back(Token(TOK_MINUS));  break;
            case L'*': m_tokens.push_back(Token(TOK_MUL));    break;
            case L'/': m_tokens.push_back(Token(TOK_DIV));    break;
            case L'^': m_tokens.push_back(Token(TOK_POW));    break;
            case L'(': m_tokens.push_back(Token(TOK_LPAREN)); break;
            case L')': m_tokens.push_back(Token(TOK_RPAREN)); break;
            default:
                m_strError.Format(_T("Unknown character '%c' at position %zu"), c, i + 1);
                return false;
        }
        ++i;
    }

    m_tokens.push_back(Token(TOK_END));
    return true;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------
bool CExpressionParser::Parse(const CString& strExpr)
{
    m_strError = _T("");
    std::wstring wexpr(strExpr.GetString());

    if (!Tokenize(wexpr))
        return false;

    // Syntax check: do a trial evaluation with x = 1.
    // Domain errors (sqrt(-1) etc.) are acceptable at this stage.
    m_pos = 0;
    m_x   = 1.0;
    try
    {
        ParseExpr();
        if (Current().type != TOK_END)
        {
            m_strError = _T("Unexpected token after expression");
            return false;
        }
    }
    catch (const SyntaxError& ex)
    {
        m_strError = CString(ex.what());
        return false;
    }
    catch (...)
    {
        // Domain errors at x=1 are not syntax errors — expression is still valid.
    }

    return true;
}

bool CExpressionParser::Evaluate(double x, double& result) const
{
    m_pos = 0;
    m_x   = x;
    try
    {
        result = ParseExpr();
        if (!_finite(result) || _isnan(result))
            return false;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// ---------------------------------------------------------------------------
// Token helpers
// ---------------------------------------------------------------------------
const CExpressionParser::Token& CExpressionParser::Current() const
{
    if (m_pos < m_tokens.size())
        return m_tokens[m_pos];
    static const Token sEnd(TOK_END);
    return sEnd;
}

void CExpressionParser::Advance() const
{
    if (m_pos < m_tokens.size())
        ++m_pos;
}

// ---------------------------------------------------------------------------
// Recursive-descent grammar
//   expr    = term (('+' | '-') term)*
//   term    = power (('*' | '/') power)*
//   power   = unary ('^' unary)*      (right-associative)
//   unary   = '-' unary | primary
//   primary = number | 'x' | 'pi' | 'e' | ident '(' expr ')' | '(' expr ')'
// ---------------------------------------------------------------------------
double CExpressionParser::ParseExpr() const
{
    double lhs = ParseTerm();
    while (Current().type == TOK_PLUS || Current().type == TOK_MINUS)
    {
        bool add = (Current().type == TOK_PLUS);
        Advance();
        double rhs = ParseTerm();
        lhs = add ? lhs + rhs : lhs - rhs;
    }
    return lhs;
}

double CExpressionParser::ParseTerm() const
{
    double lhs = ParsePower();
    while (Current().type == TOK_MUL || Current().type == TOK_DIV)
    {
        bool mul = (Current().type == TOK_MUL);
        Advance();
        double rhs = ParsePower();
        if (!mul)
        {
            if (rhs == 0.0) throw std::runtime_error("Division by zero");
            lhs /= rhs;
        }
        else
        {
            lhs *= rhs;
        }
    }
    return lhs;
}

double CExpressionParser::ParsePower() const
{
    double base = ParseUnary();
    if (Current().type == TOK_POW)
    {
        Advance();
        double exponent = ParsePower(); // right-associative: recurse for exponent
        if (base < 0.0 && exponent != std::floor(exponent))
            throw std::runtime_error("Complex result from negative base with fractional exponent");
        return std::pow(base, exponent);
    }
    return base;
}

double CExpressionParser::ParseUnary() const
{
    if (Current().type == TOK_MINUS)
    {
        Advance();
        return -ParseUnary();
    }
    return ParsePrimary();
}

double CExpressionParser::ParsePrimary() const
{
    const Token& tok = Current();

    // Number literal
    if (tok.type == TOK_NUMBER)
    {
        double val = tok.numVal;
        Advance();
        return val;
    }

    // Identifier: constant, variable, or function call
    if (tok.type == TOK_IDENT)
    {
        std::wstring name = tok.strVal;
        Advance();

        // Built-in constants
        if (name == L"pi")  return 3.14159265358979323846;
        if (name == L"e")   return 2.71828182845904523536;

        // Variable
        if (name == L"x")   return m_x;

        // Function call: name '(' expr ')'
        if (Current().type == TOK_LPAREN)
        {
            Advance(); // consume '('
            double arg = ParseExpr();
            if (Current().type != TOK_RPAREN)
                throw SyntaxError("Missing closing parenthesis");
            Advance(); // consume ')'
            return CallFunction(name, arg);
        }

        throw SyntaxError("Unknown identifier");
    }

    // Parenthesised sub-expression
    if (tok.type == TOK_LPAREN)
    {
        Advance(); // consume '('
        double val = ParseExpr();
        if (Current().type != TOK_RPAREN)
            throw SyntaxError("Missing closing parenthesis");
        Advance(); // consume ')'
        return val;
    }

    throw SyntaxError("Expected number, variable, or '('");
}

// ---------------------------------------------------------------------------
// Built-in function dispatch
// ---------------------------------------------------------------------------
double CExpressionParser::CallFunction(const std::wstring& name, double arg) const
{
    if (name == L"sin")   return std::sin(arg);
    if (name == L"cos")   return std::cos(arg);
    if (name == L"tan")
    {
        double c = std::cos(arg);
        if (std::fabs(c) < 1e-15)
            throw std::runtime_error("tan is undefined at this point");
        return std::sin(arg) / c;
    }
    if (name == L"asin" || name == L"arcsin")
    {
        if (arg < -1.0 || arg > 1.0)
            throw std::runtime_error("asin domain error: argument must be in [-1, 1]");
        return std::asin(arg);
    }
    if (name == L"acos" || name == L"arccos")
    {
        if (arg < -1.0 || arg > 1.0)
            throw std::runtime_error("acos domain error: argument must be in [-1, 1]");
        return std::acos(arg);
    }
    if (name == L"atan" || name == L"arctan") return std::atan(arg);
    if (name == L"sinh") return std::sinh(arg);
    if (name == L"cosh") return std::cosh(arg);
    if (name == L"tanh") return std::tanh(arg);
    if (name == L"exp")  return std::exp(arg);
    if (name == L"log" || name == L"ln")
    {
        if (arg <= 0.0) throw std::runtime_error("log domain error: argument must be > 0");
        return std::log(arg);
    }
    if (name == L"log10")
    {
        if (arg <= 0.0) throw std::runtime_error("log10 domain error: argument must be > 0");
        return std::log10(arg);
    }
    if (name == L"sqrt")
    {
        if (arg < 0.0) throw std::runtime_error("sqrt domain error: argument must be >= 0");
        return std::sqrt(arg);
    }
    if (name == L"abs")   return std::fabs(arg);
    if (name == L"sign")  return (arg > 0.0) ? 1.0 : (arg < 0.0) ? -1.0 : 0.0;
    if (name == L"floor") return std::floor(arg);
    if (name == L"ceil")  return std::ceil(arg);
    if (name == L"round") return std::round(arg);

    throw SyntaxError("Unknown function");
}
