# GraphDrawer

A Windows MFC application for plotting mathematical functions on a 2-D Cartesian
coordinate system.

## Features

- Plot up to **14 built-in functions** simultaneously (sine, cosine, tangent,
  cotangent, exponential, natural logarithm, arcus sine/cosine/tangent/cotangent,
  and the four hyperbolic equivalents).
- Enter an **arbitrary expression** `y = f(x)` using a recursive-descent expression
  parser that supports all common mathematical operators, functions, and constants.
- **Background thread** computation for custom expressions — the UI stays responsive.
- Customisable coordinate axes: colour, line thickness, tick marks, and labels.
- Selectable background colour.
- Print and print-preview support.
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
4. Use **Draw › Draw Options…** to change axis style, tick interval, and colours.

### Expression Syntax

| Element | Syntax |
|---------|--------|
| Variable | `x` |
| Constants | `pi`, `e` |
| Arithmetic | `+`, `-`, `*`, `/` |
| Power | `x^3`, `2^x` (right-associative) |
| Functions | `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `exp`, `log`/`ln`, `log10`, `sqrt`, `abs`, `sign`, `floor`, `ceil`, `round` |
| Grouping | Parentheses `(` `)` |

Example expressions:

```
x^2 - x + 1
x * sin(x) + cos(x) - exp(1/x)
exp(-x^2)
sqrt(abs(x)) * sign(x)
(x^2 - 1) / (x - 1)
```

Points where the expression is undefined (domain errors, division by zero) are
silently skipped, leaving a gap in the curve.

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

- Custom expression is evaluated on x ∈ [−20, +20] with step 0.0005.  Zooming or
  panning does not yet trigger re-evaluation over the new visible range.
- The built-in functions are always plotted over the full visible scroll region;
  zooming shows more or fewer periods depending on the scroll size.
- No undo/redo for Draw Options changes (document dirty-flag is set but only the
  function-selection settings are persisted to file).

## License

This project is provided as-is for educational and personal use.
