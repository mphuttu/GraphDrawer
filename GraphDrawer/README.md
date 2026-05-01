# GraphDrawer

A Windows MFC application for plotting mathematical functions on a 2-D Cartesian
coordinate system.

## Features

- Plot up to **14 built-in functions** simultaneously (sine, cosine, tangent,
  cotangent, exponential, natural logarithm, arcus sine/cosine/tangent/cotangent,
  and the four hyperbolic equivalents).
- Enter an **arbitrary expression** `y = f(x)` using a recursive-descent expression
  parser that supports all common mathematical operators, functions, and constants.
- **Parametric curves** — plot `x(t)`, `y(t)` expressions over a given t-range
  (e.g. a cycloid with `x = a*(t - sin(t))`, `y = a*(1 - cos(t))`).
- **User-defined curve library** — add multiple `y = f(x)` or parametric curves via
  the *Add…* button in the Draw Functions palette.  Each curve has its own label,
  colour, and a checkbox to toggle visibility.
- **Zoomable / pannable** coordinate view — scroll the mouse wheel to zoom in or out
  centred on the viewport; drag with the left mouse button to pan; right-click to
  reset the view to the default range.
- **Configurable axis range** — set the visible x/y range (min and max) in the
  *Draw Options* dialog instead of being limited to a fixed ±10 window.
- **Background thread** computation for custom expressions — the UI stays responsive.
- Customisable coordinate axes: colour, line thickness, tick marks, and labels.
- Selectable background colour.
- Print and print-preview support — output matches the on-screen view exactly,
  with a file-name header and page-number footer on each page.
- Builds for both **Win32 (x86)** and **x64** targets.

## System Requirements

- Windows 10 or later (Windows 7/8 may work but are untested).
- [Visual C++ 2019 Redistributable][vcredist] (x86 or x64 matching the build you run).
- MFC is linked dynamically, so the MFC DLLs must be present on the target machine
  (the Visual C++ redistributable package includes them).

[vcredist]: https://aka.ms/vs/17/release/vc_redist.x64.exe

## Building

1. Open `GraphDrawer.vcxproj` in Visual Studio 2019 or later.
2. Select the desired **Configuration** (`Debug` or `Release`) and **Platform**
   (`Win32` or `x64`).
3. Build → Build Solution (or press **F7**).

| Configuration | Platform | Output path |
|---------------|----------|-------------|
| Debug         | Win32    | `Debug\GraphDrawer.exe` |
| Release       | Win32    | `Release\GraphDrawer.exe` |
| Debug         | x64      | `Debug\x64\GraphDrawer.exe` |
| Release       | x64      | `Release\x64\GraphDrawer.exe` |

Alternatively, build from the command line:

```
msbuild GraphDrawer.vcxproj /p:Configuration=Release /p:Platform=x64
```

## Usage

1. Launch `GraphDrawer.exe`.
2. The **Draw Functions** palette appears automatically.  Tick any built-in function
   checkbox to plot it.
3. To plot a custom expression, type it in the **y =** field and tick
   **Draw Custom Function**.  Press **OK** to apply.
4. Use **Draw › Draw Options…** to change axis style, tick interval, colours, and
   the visible coordinate range (x min/max, y min/max).

### Zooming and Panning

| Action | Effect |
|--------|--------|
| Mouse wheel up | Zoom in (centred on viewport) |
| Mouse wheel down | Zoom out |
| Left-button drag | Pan |
| Right-click | Reset view to the range set in Draw Options |

### Adding User-Defined Curves

1. Open the **Draw Functions** palette.
2. Click **Add…** in the *User-Defined Curves* group.
3. In the dialog that opens:
   - Choose **y = f(x)** or **Parametric** (x(t), y(t)).
   - Enter the expression(s), the x- or t-range, an optional label, and a colour.
   - Click **OK**.
4. The new curve appears in the checkbox list.  Uncheck it to hide it; click
   **Remove** to delete it.

### Parametric Curve Examples

| Curve | x(t) | y(t) | t range |
|-------|------|------|---------|
| Circle | `cos(t)` | `sin(t)` | 0 … 2π |
| Cycloid (a = 1) | `t - sin(t)` | `1 - cos(t)` | 0 … 6π |
| Lissajous | `sin(3*t)` | `sin(2*t + pi/4)` | 0 … 2π |

### Expression Syntax

| Element | Syntax |
|---------|--------|
| Variable | `x` (for y=f(x) curves) or `t` (for parametric curves) |
| Constants | `pi`, `e` |
| Arithmetic | `+`, `-`, `*`, `/` |
| Power | `x^3`, `2^x` (right-associative) |
| Functions | `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `exp`, `log`/`ln`, `log10`, `sqrt`, `abs`, `sign`, `floor`, `ceil`, `round` |
| Grouping | Parentheses `(` `)` |

## Uutta 2026-04
- Polaarikäyrät: Käyttäjä voi lisätä käyriä muodossa r = f(φ), esim. spiraalit ja ruusukäyrät.
- Implisiittikäyrät: Käyttäjä voi lisätä käyriä muodossa f(x, y) = 0, esim. Astroidi, ympyrä, ellipsi.

## Uutta 2026-05 — Asteikkotilat ja pikselipohjainen piirto

### Kolme asteikkotilaa (*Draw Options › Scale Mode*)

| Tila | Kuvaus |
|------|--------|
| **Equal Scale** | X- ja Y-akseleilla on fyysisesti sama yksikkökoko pikseleinä. Ympyrä `x² + y² = 1` näkyy täydellisenä ympyränä — ei ellipsinä. |
| **Free Scale** | Käyttäjä asettaa erikseen X- ja Y-asteikon senttimetreissä per matemaattinen yksikkö. |
| **Semi-Log** | Y-akseli on logaritminen, X-akseli lineaarinen (puolilogaritminen asteikko). |

### Tekninen toteutus
- Näyttöpiirto käyttää nyt **pikselikoordinaatteja** (Windows `MM_TEXT`) suoraan
  `GetClientRect`:stä — ei enää `MM_LOMETRIC`-muunnosta, joka antoi epäluotettavan
  kuvasuhteen non-square-DPI-näytöillä.
- Tulostus käyttää edelleen `MM_LOMETRIC`-tilaa.
- Equal Scale -tilassa `aspect = w/h` laajennetaan pienempi akseli niin, että
  1 matemaattinen yksikkö vie täsmälleen saman pikselimäärän molemmissa suunnissa.
- Tikutusaskel (`niceStep`) pakotetaan samaksi X- ja Y-akseleille kun skaalat ovat
  yhtä suuret, jotta tikut näyttävät fyysisesti yhtä tiheiltä.
- Hiirella panorointi käyttää oikeaa pikseliä/yksikkö-suhdetta kaikissa tiloissa.

## Help

The compiled help file `Help\GraphDrawer.chm` can be opened from the application's
**Help** menu or directly from Windows Explorer.

To recompile the help file from sources (requires
[HTML Help Workshop](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/htmlhelp/microsoft-html-help-downloads)):

```
cd Help
hhc GraphDrawer.hhp
```

## Known Limitations

- The custom `y = f(x)` background-thread expression is re-evaluated when the
  expression text or x-range changes; zooming does not trigger re-evaluation —
  use the **x-range** fields in Draw Options to control the sampling domain.
- No undo/redo for Draw Options changes (document dirty-flag is set but only the
  function-selection settings are persisted to file).

## License

This project is provided as-is for educational and personal use.
