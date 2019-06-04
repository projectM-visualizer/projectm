; This script requires Inno Setup Compiler 5.5.9 or later to compile
; The Inno Setup Compiler (and IDE) can be found at http://www.jrsoftware.org/isinfo.php

; General documentation on how to use InnoSetup scripts: http://www.jrsoftware.org/ishelp/index.php

#ifndef Platform
  #error Platform undefined. Pass /DPlatform
#endif

#ifndef Configuration
  #error Configuration undefined. Pass /DConfiguration
#endif

#define MyAppName "projectM"
#define MyAppInstallerVersion GetFileVersion("projectM.exe")
#define MyAppPublisher "projectM Team"
#define MyAppPublisherURL "https://github.com/mancoast/projectm"
#define MyAppURL "https://github.com/projectM-visualizer/projectm"
#define MyAppExeName "projectM.exe"
#define MyAppIcoName "projectm_qDj_icon.ico"

[Setup]
AppId={{82DAD0F1-77DF-47FE-9FEE-123FFC5695B4}
AppName={#MyAppName}
AppVersion={#MyAppInstallerVersion}
VersionInfoVersion={#MyAppInstallerVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppPublisherURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppCopyright=Copyright (C) 2019 {#MyAppPublisher}
BackColor=clWhite
BackSolid=yes
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename=projectM-v{#MyAppInstallerVersion}-Setup
OutputDir=..\..\Setup\{#Configuration}\{#Platform}
Compression=lzma2
InternalCompressLevel=ultra64
SolidCompression=yes
DisableDirPage=yes
DisableReadyPage=yes
SetupIconFile={#MyAppIcoName}
WizardImageStretch=no
WindowResizable=no
CloseApplications=yes
ChangesEnvironment=yes
ArchitecturesInstallIn64BitMode=x64
LicenseFile=LICENSE.txt
WizardImageFile=setup_inno.bmp
WizardSmallImageFile=setup_inno_small.bmp

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; \
    GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Icons]
Name: "{group}\projectM"; Filename: "{app}\projectM.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall projectM"; Filename: "{uninstallexe}"
Name: "{userdesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; \
    IconFilename: "{app}\{#MyAppIcoName}"; Tasks: desktopicon

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl";

[Types]
Name: "full"; Description: "Full installation"

[Components]

[Files]
DestDir: "{app}"; Flags: ignoreversion recursesubdirs; Source:"*"

[UninstallDelete]
; Deletes the entire installation directory, including files and subdirectories
Type: filesandordirs; Name: "{app}";

[Run]
Filename: "{app}\projectM.exe"; Description: "Launch projectM application"; \
    Flags: postinstall nowait skipifsilent unchecked

