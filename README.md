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
   - Choose **y = f(x)**, **Parametric**, **Polar**, or **Implicit**.
   - Enter the expression(s), the range, an optional label, and a colour.
   - Range fields accept constant expressions: `2*pi`, `-pi`, `e^2`, etc.
   - Click **OK**.
4. The new curve appears in the checkbox list.  Uncheck it to hide it.
5. To **edit** an existing curve: select it in the list and click **Edit…** — the
   dialog reopens pre-filled; confirm with **OK** to apply changes.
6. Click **Remove** to delete the selected curve.

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
| Power | `x^3`, `2^x` (right-associative; unary `-` has lower precedence, so `-x^2` = `-(x^2)`) |
| Functions | `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `exp`, `log`/`ln`, `log10`, `sqrt`, `abs`, `sign`, `floor`, `ceil`, `round` |
| Grouping | Parentheses `(` `)` |

## What's New in 2026-05 (latest)

### Stability and correctness fixes

- **Print Preview header no longer shows garbled characters.**  The header font
  was previously sized in screen pixels but rendered in MM_LOMETRIC units, making
  it illegibly small (≈ 1.6 mm tall on a 96 dpi display).  The font height is now
  specified in proper MM_LOMETRIC logical units (≈ 4.2 mm ≈ 12 pt).  In addition,
  the Y coordinate of the header and footer text was computed with the wrong sign
  (MM_LOMETRIC Y grows upward, not downward), so both texts were drawn outside the
  printable area.  Both offsets are now correct.
- **Crash after prolonged use with multiple user-defined curves fixed.**  Every
  repaint of the custom `y = f(x)` curve selected a GDI pen object into the DC
  but never deselected it before the pen went out of scope.  `DeleteObject` fails
  silently on a pen that is still selected, so one GDI handle leaked per repaint.
  Windows limits a process to ≈ 10 000 GDI handles; after enough zoom/pan operations
  the pool was exhausted and the application crashed.  The old pen is now saved and
  restored before the `CPen` is destroyed.
- **Worker-thread use-after-free fixed.**  The background expression-evaluation
  thread was launched with `m_bAutoDelete = TRUE`, which lets MFC delete the
  `CWinThread` object as soon as the thread exits.  If the thread finished before
  `SetCustomExpression` or the document destructor called
  `WaitForSingleObject(m_pDrawThread->m_hThread, …)`, the handle was read from
  already-freed memory (undefined behaviour, potential crash).
  `m_bAutoDelete` is now `FALSE`; the `CWinThread` object is explicitly `delete`d
  after the wait completes.

### Expression parser fixes and improvements
- **Negative exponents now work correctly**: `exp(-x^2)` is now evaluated as
  `exp(-(x^2))`, i.e. unary minus has lower precedence than `^`.
  Previously `-x^2` was incorrectly treated as `(-x)^2`.
- **`pi` and `e` in range fields**: all numeric input fields that accept a range
  value (x from/to, t from/to, φ from/to, implicit x/y from/to, and the
  custom-expression x-range) now accept arbitrary constant expressions such as
  `2*pi`, `-pi/2`, or `e^2` in addition to plain numbers.

### User-defined curve editing
- **Edit… button** added to the *User-Defined Curves* group in the Draw Functions
  palette.  Select a curve in the list and click **Edit…** to re-open the
  *Add Curve* dialog pre-filled with the curve's current settings.  Confirm with
  **OK** to replace the curve in-place, or **Cancel** to leave it unchanged.

## What's New in 2026-04
- **Polar curves**: add curves of the form r = f(φ), e.g. spirals and rose curves.
- **Implicit curves**: add curves of the form f(x, y) = 0, e.g. astroid, circle, ellipse.

## What's New in 2026-05 — Scale Modes and Pixel-Based Rendering

### Three Scale Modes (*Draw Options › Scale Mode*)

| Mode | Description |
|------|-------------|
| **Equal Scale** | Both axes use the same number of pixels per mathematical unit, regardless of the monitor's DPI or aspect ratio.  A circle `x² + y² = 1` is drawn as a true circle, not an ellipse. |
| **Free Scale** | Set independent X and Y scales in centimetres per unit via the *Scale X* and *Scale Y* fields. |
| **Logarithmic** | One or both axes are logarithmic (base 10).  Select the sub-mode with the radio buttons next to the *Logarithmic scale* option: **Log X, Linear Y** / **Linear X, Log Y** / **Log X, Log Y**.  Axis values must be strictly positive. |

### Technical Notes
- Screen rendering now uses **pixel coordinates** (`MM_TEXT`) obtained directly
  from `GetClientRect` — no more `MM_LOMETRIC` conversion, which could produce
  incorrect aspect ratios on non-square-DPI monitors.
- Print rendering still uses `MM_LOMETRIC`.
- In Equal Scale mode, `aspect = w/h` expands the shorter axis so that
  1 mathematical unit spans exactly the same number of pixels in both directions.
- The tick step (`niceStep`) is forced to the same value for both axes when the
  scales are equal, so tick marks appear physically equally dense in both directions.
- Mouse panning uses the correct pixels-per-unit ratio in every scale mode.

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
