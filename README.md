# GraphDrawer

GraphDrawer is a Windows desktop application for plotting mathematical
functions and geometric constructions on a two-dimensional coordinate plane.
It is written in C++ with Microsoft Foundation Classes (MFC).

## Features

- Fourteen built-in functions, including trigonometric, inverse trigonometric,
  exponential, logarithmic and hyperbolic functions.
- A numerical expression parser for custom `y = f(x)` expressions.
- User-defined explicit, parametric, polar and implicit curves, with labels,
  colors and visibility controls.
- Geometric line segments and triangles, including construction lines, angle
  labels, circumcircles and incircles.
- Equal, free and base-10 logarithmic coordinate scales; configurable axes,
  ranges and colors.
- Mouse-wheel zoom, left-button panning and right-click view reset.
- Document save/open, print and print-preview support.
- Windows HTML Help (`.chm`) documentation, accessible from **Help > Help
  Contents** or **F1**.

## Requirements

### To run GraphDrawer

- Windows 10 or later.
- Microsoft Visual C++ 2015-2022 Redistributable matching the executable's
  architecture.
- The corresponding MFC runtime. The application links MFC dynamically.

### To build GraphDrawer

- Visual Studio with the **Desktop development with C++** workload.
- MFC and a compatible Windows SDK.
- For the installer, Inno Setup 6.
- For the CHM help file, Microsoft HTML Help Workshop.

## Build the application

Open `GraphDrawer.sln`, select the **Release** configuration and **x64**
platform, then build the solution. The Release x64 output is configured as:

```text
GraphDrawer\Release\x64\GraphDrawer.exe
```

From a Visual Studio Developer PowerShell, run:

```powershell
MSBuild.exe GraphDrawer.sln /m /p:Configuration=Release /p:Platform=x64 /t:Build
```

The project also supports Debug and Win32 configurations. Those configurations
use their configured output directories.

## Build the CHM help file

Help source and the HTML Help Workshop project are in `Help`. Install
[Microsoft HTML Help Workshop][html-help-workshop], then run this from the
repository root in PowerShell:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Help\build-chm.ps1
```

The compiled file is `Help\GraphDrawer.chm`. To compile manually, open
`Help\GraphDrawer.hhp` in HTML Help Workshop or run `hhc.exe` from the `Help`
directory.

The Help Contents tab provides these primary topics:

1. Welcome to GraphDrawer
2. Getting Started
3. Customization
4. Troubleshooting
5. Frequently Asked Questions
6. Technical Information
7. Release Notes

## Build the installer

1. Build the Release x64 application.
2. Build the CHM help file.
3. Compile `Setup\GraphDrawer_Setup.iss` with [Inno Setup 6][inno-setup] (ISCC).

The installer is written to `Setup\Setup\Output\GraphDrawer_Setup_1.0_x64.exe`.
It installs the application, CHM help file and this README. The Visual C++
Redistributable is not bundled; install it separately on systems that need it.

## Quick start

1. Launch `GraphDrawer.exe`; the Draw Functions palette opens automatically.
2. Check any built-in function, or enter an expression such as `x^2 - x + 1`
   in the `y =` field and enable **Draw Custom Function**.
3. Click **Add...** to create a named explicit, parametric, polar or implicit curve.
4. Choose **Draw > Draw Options...** to set the visible range and scale.
5. Use the mouse wheel to zoom, drag with the left mouse button to pan, and
   right-click to reset the view.
6. Choose **Draw > Draw Geometric Figures** to add segments or triangles.

Expression function arguments use radians. Supported operators include `+`,
`-`, `*`, `/` and `^`; multiplication must be explicit (`2*x`). Constants
include `pi` and `e`. Available functions include `sin`, `cos`, `tan`, `asin`,
`acos`, `atan`, `sinh`, `cosh`, `tanh`, `exp`, `log`/`ln`, `log10`, `sqrt`,
`abs`, `sign`, `floor`, `ceil` and `round`.

## Document format

GraphDrawer documents use the `.gra` extension. A document stores drawing
options, enabled built-in functions, curve definitions and geometric objects.
Computed curve points are recalculated when the document is opened.

## Known limitations

- Curves are plotted by numerical sampling. Very narrow features or
  discontinuities may be missed or appear as gaps.
- The quick custom `y = f(x)` plot is sampled over its configured x-range;
  zooming does not extend that range.
- Logarithmic axes display only positive values.
- Printing is limited to one page.

## Release notes

See **Help > Help Contents > Release Notes** for the documented feature history.
The project README and the CHM are maintained as user and developer references;
the CHM is included in the installer.

## License

This project is provided as-is for educational and personal use.

[html-help-workshop]: https://learn.microsoft.com/en-us/previous-versions/windows/desktop/htmlhelp/microsoft-html-help-downloads
[inno-setup]: https://jrsoftware.org/isdl.php
