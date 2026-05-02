# C++ Build Tools Upgrade Plan

## Overview

**Target**: Resolve the C4244 warnings introduced or surfaced by the upgraded MSVC build tools.
**Scope**: Single MFC project plus solution-level and project-level configuration cleanup discovered during validation.

## Tasks

### 01-fix-c4244-warnings: Fix C4244 warnings in graph drawing code

Review the repeated drawing and plotting calculations in `GraphDrawerDoc.cpp`, update intentional floating-point-to-integer conversions to be explicit, and keep the existing rendering behavior intact. The work is limited to the warning-producing file because the rebuild reported no other issues.

**Done when**: `GraphDrawerDoc.cpp` no longer produces any C4244 warnings, the solution builds successfully with an incremental build, and a final full rebuild completes with zero errors and zero warnings.

---

### 02-fix-sln-config-mappings: Fix incorrect solution configuration mappings

Review the solution configuration table in `D:\Projects\MFC\GraphDrawer\GraphDrawer.sln`, correct any `Win32` configuration mappings that incorrectly target `Release|x64`, and keep the intended `x64` solution mappings unchanged. Validate that the corrected solution file still builds for the affected solution configurations.

**Done when**: `GraphDrawer.sln` maps `Debug|Win32`, `Release|Win32`, `Debug|x64`, and `Release|x64` to the matching project configurations, and the relevant solution builds complete successfully.

---

### 03-fix-release-x64-output-dir: Fix Release x64 output directory

Review the `Release|x64` output settings in `D:\Projects\MFC\GraphDrawer\GraphDrawer\GraphDrawer.vcxproj`, change the output directory to the project-local `GraphDrawer\Release\x64\` location requested by the user, and validate that the `Release|x64` build still succeeds and emits the executable under the requested folder.

**Done when**: `Release|x64` uses `D:\Projects\MFC\GraphDrawer\GraphDrawer\Release\x64\` as its output directory, the project file validates and reloads successfully, and a `Release|x64` build succeeds.

---
