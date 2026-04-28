# C++ Build Tools Upgrade Plan

## Overview

**Target**: Resolve the C4244 warnings introduced or surfaced by the upgraded MSVC build tools.
**Scope**: Single MFC project, one source file with repeated numeric narrowing conversions in graph drawing logic.

## Tasks

### 01-fix-c4244-warnings: Fix C4244 warnings in graph drawing code

Review the repeated drawing and plotting calculations in `GraphDrawerDoc.cpp`, update intentional floating-point-to-integer conversions to be explicit, and keep the existing rendering behavior intact. The work is limited to the warning-producing file because the rebuild reported no other issues.

**Done when**: `GraphDrawerDoc.cpp` no longer produces any C4244 warnings, the solution builds successfully with an incremental build, and a final full rebuild completes with zero errors and zero warnings.

---
