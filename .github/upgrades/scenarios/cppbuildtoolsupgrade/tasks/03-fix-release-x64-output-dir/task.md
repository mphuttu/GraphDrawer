# 03-fix-release-x64-output-dir: Fix Release x64 output directory

Review the `Release|x64` output settings in `D:\Projects\MFC\GraphDrawer\GraphDrawer\GraphDrawer.vcxproj`, change the output directory to the project-local `GraphDrawer\Release\x64\` location requested by the user, and validate that the `Release|x64` build still succeeds and emits the executable under the requested folder.

## Research
- **Affected project file**: `D:\Projects\MFC\GraphDrawer\GraphDrawer\GraphDrawer.vcxproj`
- **Current `Release|x64` output directory**: `$(SolutionDir)$(Configuration)\x64\`
- **Requested output directory**: `D:\Projects\MFC\GraphDrawer\GraphDrawer\Release\x64\`
- **Related setting**: `IntDir` is already project-local as `GraphDrawer\$(Configuration)\x64\`
- **Planned change**: Update only the `Release|x64` `OutDir` so the build output moves under the project folder without changing other configurations.

**Done when**: `Release|x64` uses `D:\Projects\MFC\GraphDrawer\GraphDrawer\Release\x64\` as its output directory, the project file validates and reloads successfully, and a `Release|x64` build succeeds.
