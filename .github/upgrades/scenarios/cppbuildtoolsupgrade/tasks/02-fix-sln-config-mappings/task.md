# 02-fix-sln-config-mappings: Fix incorrect solution configuration mappings

Review the solution configuration table in `D:\Projects\MFC\GraphDrawer\GraphDrawer.sln`, correct any `Win32` configuration mappings that incorrectly target `Release|x64`, and keep the intended `x64` solution mappings unchanged. Validate that the corrected solution file still builds for the affected solution configurations.

## Research
- **Affected solution file**: `D:\Projects\MFC\GraphDrawer\GraphDrawer.sln`
- **Affected project**: `D:\Projects\MFC\GraphDrawer\GraphDrawer\GraphDrawer.vcxproj`
- **Problem found**: In `ProjectConfigurationPlatforms`, the `Debug|Win32` and `Release|Win32` solution mappings incorrectly point to `Release|x64` instead of their matching `Win32` project configurations.
- **Expected mapping**: Each solution configuration should map to the corresponding project configuration with the same configuration/platform pair.
- **Validation target**: Confirm that `Release|x64` still builds successfully and that the corrected `Win32` solution mappings no longer redirect to `x64`.

**Done when**: `GraphDrawer.sln` maps `Debug|Win32`, `Release|Win32`, `Debug|x64`, and `Release|x64` to the matching project configurations, and the relevant solution builds complete successfully.
