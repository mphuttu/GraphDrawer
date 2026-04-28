# 01-fix-c4244-warnings: Fix C4244 warnings in graph drawing code

Review the repeated drawing and plotting calculations in `GraphDrawerDoc.cpp`, update intentional floating-point-to-integer conversions to be explicit, and keep the existing rendering behavior intact. The work is limited to the warning-producing file because the rebuild reported no other issues.

## Research
- **Affected project**: `D:\Projects\MFC\GraphDrawer\GraphDrawer\GraphDrawer.vcxproj`
- **Affected file**: `D:\Projects\MFC\GraphDrawer\GraphDrawer\GraphDrawerDoc.cpp`
- **Build signal**: The rebuild reported 74 C4244 warnings and 0 errors, all in the same source file.
- **Pattern found**: The warnings come from repeated assignments of trigonometric and logarithmic plotting expressions to integer pixel coordinates and loop bounds, such as `EndX`, `nEndY`, `nCount`, `xpos`, `xposneg`, `ypos`, `yposneg`, `ytpos`, and `ytposneg`.
- **Planned fix**: Preserve existing behavior by making the narrowing conversions explicit with a local helper that performs `static_cast<int>`.

**Done when**: `GraphDrawerDoc.cpp` no longer produces any C4244 warnings, the solution builds successfully with an incremental build, and a final full rebuild completes with zero errors and zero warnings.
