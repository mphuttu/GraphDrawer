; GraphDrawer Inno Setup Script
; Requires Inno Setup 6 (https://jrsoftware.org/isinfo.php)
;
; Build the x64 Release before running this script:
;   msbuild GraphDrawer.vcxproj /p:Configuration=Release /p:Platform=x64
;
; The resulting installer is placed in the Setup\Output\ folder.

#define AppName      "GraphDrawer"
#define AppVersion   "1.0"
#define AppPublisher "Mika Huttunen"
#define AppExe       "GraphDrawer.exe"
#define AppURL       ""

[Setup]
AppId={{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
AllowNoIcons=yes
; Require 64-bit Windows
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
; Output
OutputDir=Setup\Output
OutputBaseFilename=GraphDrawer_Setup_{#AppVersion}_x64
Compression=lzma2/ultra64
SolidCompression=yes
; Appearance
WizardStyle=modern
; Minimum Windows version: Windows 10 (6.2 or later accepted for compatibility)
MinVersion=6.1

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main executable (x64 Release build)
Source: "..\Release\x64\{#AppExe}"; DestDir: "{app}"; Flags: ignoreversion

; Visual C++ 2019 / 2022 Redistributable (x64)
; Bundle the redistributable so the end user doesn't need to install it separately.
; Download vc_redist.x64.exe from https://aka.ms/vs/17/release/vc_redist.x64.exe
; and place it next to this .iss file before compiling the installer.
;Source: "vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

; Help file
Source: "..\Help\GraphDrawer.chm"; DestDir: "{app}\Help"; Flags: ignoreversion

; README
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion isreadme

[Icons]
Name: "{group}\{#AppName}";         Filename: "{app}\{#AppExe}"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#AppName}";   Filename: "{app}\{#AppExe}"; Tasks: desktopicon

[Run]
; Launch GraphDrawer after install (optional)
Filename: "{app}\{#AppExe}"; Description: "{cm:LaunchProgram,{#StringChange(AppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

; Install Visual C++ Redistributable silently (uncomment if you bundle vc_redist)
;Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /passive /norestart"; StatusMsg: "Installing Visual C++ Redistributable..."; Check: NeedsVCRedist; Flags: waituntilterminated

[Code]
(*
  Optional helper: check if the VC++ 2015-2022 x64 Redistributable is already installed.
  Uncomment the [Run] entry above and this function if you bundle vc_redist.x64.exe.
*)
(*
function NeedsVCRedist: Boolean;
var
  dwVersion: Cardinal;
begin
  // Check for VC++ 2015-2022 x64 runtime
  Result := not RegQueryDWordValue(
    HKLM,
    'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
    'Installed',
    dwVersion) or (dwVersion = 0);
end;
*)
