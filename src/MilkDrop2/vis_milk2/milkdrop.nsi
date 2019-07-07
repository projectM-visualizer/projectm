;   LICENSE
;   -------
; Copyright 2005-2013 Nullsoft, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without modification, 
; are permitted provided that the following conditions are met:
; 
;   * Redistributions of source code must retain the above copyright notice,
;     this list of conditions and the following disclaimer. 
; 
;   * Redistributions in binary form must reproduce the above copyright notice,
;     this list of conditions and the following disclaimer in the documentation
;     and/or other materials provided with the distribution. 
; 
;   * Neither the name of Nullsoft nor the names of its contributors may be used to 
;     endorse or promote products derived from this software without specific prior written permission. 
;  
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
; FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
; CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
; IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
; OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; ----------------------------------------------------------------
; NOTE: this .nsi script was last built with 'makensis.exe' 
; version 1.98.  If you go to install it (unchanged) and get errors,
; try tracking down that older version, or try to figure out what
; has changed since then.  You can get NSIS at:
;
;               http://www.nullsoft.com/free/nsis/
; 
; This installer will produce a single EXE file that, when run, 
; will decompress and install your plugin and all of its data files 
; to Winamp 2 or Winamp 3.
; ----------------------------------------------------------------
; MODIFYING THIS SCRIPT TO CREATE AN INSTALLER FOR YOUR OWN PLUGIN:
; ----------------------------------------------------------------
;   1) there are three sections below, each marked 'EDIT THIS SECTION 
;        [x/3]' where 'x' is the section number.  These are the 3
;        places where you need to make changes to customize this
;        installer script for your particular plugin.  Go to each
;        and make the changes, reading the comments there for
;        more information.
;   2) download NSIS from http://www.nullsoft.com/free/nsis/, if you
;        haven't already.
;   3) run the command 'makensis.exe installer.nsi' to build
;        the executable.  (note: ignore the warning message about
;        'InstallRegKey' being used multiple times; this is necessary
;        to determine whether Winamp 2 or 3 is a better candidate
;        for the install.)
; ----------------------------------------------------------------
; WHAT THIS INSTALLER SCRIPT DOES:
; ----------------------------------------------------------------
;   If Winamp 2 is installed, it will install your plugin to Winamp 2, 
;   in the directory Winamp\Plugins (or whatever is specified as the 
;   vis plugins path, in Winamp\winamp.ini).  It will also select
;   the plugin as the current plugin.  (Note that Winamp must be
;   closed to do this, so if it's open, the installer will ask the 
;   user to close it before proceeding.)  At the end of a successful
;   install, it asks if they'd like to run Winamp.
; 
;   If Winamp 2 is not present but Winamp 3 is, or if the user manually
;   selects the Winamp 3 directory to install to, the plugin will be 
;   installed to Winamp 3 as a classic visualization plugin, to the 
;   directory Winamp3\Plugins.  At install time, if ClassicVis is not
;   installed, it will prompt the user to go download it.  If they
;   don't download it, it will tell them the installation failed.
;   If they already had it, or after they presumably download it,
;   the installer will briefly tell them how to select their new 
;   plugin and run it in Winamp 3, using ClassicVis.  Finally, at 
;   the end of a successful install, it asks if they'd like to run 
;   Winamp 3.
; ----------------------------------------------------------------



; -------------------- EDIT THIS SECTION [1/3] --------------------
; -------------------- EDIT THIS SECTION [1/3] --------------------
; -------------------- EDIT THIS SECTION [1/3] --------------------

  !define PLUGIN_NAME     "MilkDrop 2"                  ; Brief name of the component.  Can have spaces in it.
  !define INSTALL_CAPTION "MilkDrop 2.2 Setup"       ; Caption for the installer dialog
  !define PLUGIN_DLL      "vis_milk2.dll"              ; The filename of the actual plugin
  !define PLUGIN_OUTFILE  "milkdrop_2.exe"          ; Name of the installer to create

; ----------------------- END SECTION [1/3] -----------------------
; ----------------------- END SECTION [1/3] -----------------------
; ----------------------- END SECTION [1/3] -----------------------

; ----------------------------------------------------------------
Name ${PLUGIN_NAME}
Caption "${INSTALL_CAPTION}"
OutFile ${PLUGIN_OUTFILE}
; ----------------------------------------------------------------

; to determine the install directory, we start with Program Files\Winamp.
; then, if winamp 3 is found, override this and use that directory.
; finally, if winamp 2 is found, override again and use that directory.
InstallDir $PROGRAMFILES\Winamp
InstallDirRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp3" "UninstallString"
InstallDirRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp" "UninstallString"
DirText \
    "The installer has determined that this is the path to Winamp.$\rIf it is not correct, please change it.  You will not be able to proceed$\runtil a valid path is found." \
    "Select the directory in which Winamp 2 or 3 is installed:"

Function .onInit
  IfFileExists $INSTDIR\winamp.exe End      ; else if winamp.exe found (winamp2), we're good
  IfFileExists $INSTDIR\winamp3.exe End     ; if winamp3.exe found (winamp3), we're good
  IfFileExists $INSTDIR\studio.exe End      ; or if studio.exe found (older version of winamp3), we're good
  ; the above will work fine if winamp 2 or 3 is installed,
  ; but will break if winamp 2 or 3 was uninstalled *but the directory remains.*
  IfFileExists $PROGRAMFILES\Winamp\winamp.exe SelectNaturalWinamp2         ; last but not least, try the default winamp 2 directory
  IfFileExists $PROGRAMFILES\Winamp3\winamp3.exe SelectNaturalWinamp3       ; otherwise, try the default winamp 3 directory
  IfFileExists $PROGRAMFILES\Winamp3\studio.exe SelectNaturalWinamp3        ; otherwise, try the default winamp 3 directory
  ; if all of these failed, no good path to Winamp (2 or 3) could be found,
  ; and the 'Next' button will be disabled until the user can specify
  ; the correct folder.
  Goto End
  
  SelectNaturalWinamp3:
    strcpy $INSTDIR $PROGRAMFILES\Winamp3
    goto End
    
  SelectNaturalWinamp2:
    strcpy $INSTDIR $PROGRAMFILES\Winamp
    goto End  
  
  End:
FunctionEnd


Function .onVerifyInstDir
  IfFileExists $INSTDIR\Winamp.exe DirOk
  IfFileExists $INSTDIR\Winamp3.exe DirOk
  IfFileExists $INSTDIR\Studio.exe DirOk
  Abort   ; leaves the directory as the selected one, but disables the 'Next' button...
  
  DirOk:
FunctionEnd


  
  

Function QueryWinampVisPath 
  ; input: $INSTDIR, the currently-selected install dir (path to winamp)
  ; output: $1, the path to the winamp vis plugins subdirectory
  ;           -for winamp 3x, this is just $INSTDIR\plugins
  ;           -for winamp 2x, it comes from the winamp.ini file
  ;             (or just $INSTDIR\plugins if there is an error reading it.)
  IfFileExists $INSTDIR\Winamp.exe CaseWinamp2
  IfFileExists $INSTDIR\Winamp3.exe CaseWinamp3
  IfFileExists $INSTDIR\Studio.exe CaseWinamp3  ; legacy check
  goto CaseImpossible
          
  CaseWinamp2:
    StrCpy $1 $INSTDIR\Plugins\MilkDrop2
    ReadINIStr $8 $INSTDIR\winamp.ini Winamp VisDir
    StrCmp $8 "" End
    IfFileExists $8 0 End
      StrCpy $1 $8 ; update dir
    goto end
  
  CaseWinamp3: 
  CaseImpossible:
    StrCpy $1 $INSTDIR\Plugins\MilkDrop2
    goto end
    
  End: 
FunctionEnd


; The stuff to install
Section ""

  CloseWinamp2:
    ; make sure winamp is closed before we 1) try to install files
    ; and 2) (later) edit winamp.ini.  for 1), if they're running
    ; (or were running) some other vms-based plugin using 
    ; vms_desktop.dll, then if winamp is still open, the installer
    ; could have trouble overwriting vms_desktop.dll, or other files;
    ; the user would get an abort/retry/ignore box, but it's easier 
    ; to just play it safe.
    FindWindow $R0 "winamp v1.x"
    StrCmp $R0 0 "" RequestCloseWinamp2
    goto Winamp2Closed
  RequestCloseWinamp2:
    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "Winamp is currently running; please close it,$\rand then hit OK to continue..." \
        IDCANCEL WarnWinampStillOpen
    goto CloseWinamp2
  WarnWinampStillOpen:
    FindWindow $R0 "winamp v1.x"
    StrCmp $R0 0 "" WarnWinampStillOpen2
    goto Winamp2Closed
  WarnWinampStillOpen2:
    MessageBox MB_OK|MB_ICONEXCLAMATION "Warning: Winamp is still open; as a result, the installer$\rwill not be able to set ${PLUGIN_NAME} as the default plugin; you will$\rhave to do this yourself.$\r$\rTo do so, wait until the installation is finished.  Then bring up $\rWinamp and hit CTRL+K.  From there, you will be able to select$\r${PLUGIN_NAME} from the list of visualization plug-ins, and it will$\rbecome the new default."
    goto Winamp2Closed
  Winamp2Closed:    

  ; this function sets $1 to point to the 'winamp\plugins\milkdrop2' folder:
  ; (see function definition above)
  Call QueryWinampVisPath

  ; -------------------- EDIT THIS SECTION [2/3] --------------------
  ; -------------------- EDIT THIS SECTION [2/3] --------------------
  ; -------------------- EDIT THIS SECTION [2/3] --------------------

  ; LIST FILES TO INCLUDE WITH THE INSTALLER

  ; For each file we want to install onto the destination system,
  ; we first set the output directory (relative to $1, which is
  ; the PLUGINS directory) and then list files.  The paths for
  ; the files will be local paths on your hard disk, but fear not -
  ; the files will be placed in the current output directory 
  ; (as last set by SetOutPath) on the destination system.
  
  ; So, first, we set the current output path (the folder to which 
  ; files will be decompressed on the user's system) to '$1', 
  ; which is the path to their winamp plugins folder.
  
  SetOutPath $1
  File "C:\program files\winamp\plugins\${PLUGIN_DLL}"
  
  SetOutPath $1\Milkdrop2\config
  File "C:\program files\winamp\plugins\milkdrop2\config\milk_msg.ini"
  File "C:\program files\winamp\plugins\milkdrop2\config\milk_img.ini"
  ;File "C:\program files\winamp\plugins\milkdrop2\config\milkdrop.ini"  ;this one will be generated - do not install

  SetOutPath $1\Milkdrop2\data
  File "C:\program files\winamp\plugins\milkdrop2\data\vms_desktop.dll"
  File "C:\program files\winamp\plugins\milkdrop2\data\comp_ps.fx"
  File "C:\program files\winamp\plugins\milkdrop2\data\comp_vs.fx"
  File "C:\program files\winamp\plugins\milkdrop2\data\warp_ps.fx"
  File "C:\program files\winamp\plugins\milkdrop2\data\warp_vs.fx"
  File "C:\program files\winamp\plugins\milkdrop2\data\include.fx"

  SetOutPath $1\Milkdrop2\docs
  File "C:\program files\winamp\plugins\milkdrop2\docs\milkdrop.html"
  File "C:\program files\winamp\plugins\milkdrop2\docs\milkdrop_preset_authoring.html"
  File "C:\program files\winamp\plugins\milkdrop2\docs\q_and_t_vars.gif"

  SetOutPath $1\Milkdrop2\textures
  File "C:\program files\winamp\plugins\milkdrop2\textures\*.*"
  
  SetOutPath $1\Milkdrop2\presets
  File "C:\program files\winamp\plugins\milkdrop2\presets\*.milk"
  
  hmmm
  SetOutPath $1\MilkDrop2\presets\3d
  File "C:\program files\winamp\plugins\milkdrop2\presets\3d\*.milk"
  
  hmmm
  SetOutPath $1\MilkDrop2\presets\inverted
  File "C:\program files\winamp\plugins\milkdrop2\presets\inverted\*.milk"
  
  ; ----------------------- END SECTION [2/3] -----------------------
  ; ----------------------- END SECTION [2/3] -----------------------
  ; ----------------------- END SECTION [2/3] -----------------------
  

  ; now time to create the Uninstaller:

  IfFileExists $INSTDIR\Winamp.exe UninstWinamp2
  IfFileExists $INSTDIR\Winamp3.exe UninstWinamp3
  IfFileExists $INSTDIR\Studio.exe UninstWinamp3  ; legacy check
  goto UninstDone

  UninstWinamp3:
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}Winamp3 "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}Winamp3 "Install_Plugins_Dir" $1
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp3" "DisplayName" "${PLUGIN_NAME} for Winamp 3 (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp3" "UninstallString" '"$INSTDIR\uninst-${PLUGIN_DLL}.exe"'
    WriteUninstaller "$INSTDIR\uninst-${PLUGIN_DLL}.exe"
    CreateDirectory "$SMPROGRAMS\Winamp3\Vis Plugin Uninstallers"
    CreateShortCut "$SMPROGRAMS\Winamp3\Vis Plugin Uninstallers\Uninstall ${PLUGIN_NAME}.lnk" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" "" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" 0
    goto UninstDone
  
  UninstWinamp2:
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}Winamp "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}Winamp "Install_Plugins_Dir" $1
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp" "DisplayName" "${PLUGIN_NAME} for Winamp 2x (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp" "UninstallString" '"$INSTDIR\uninst-${PLUGIN_DLL}.exe"'
    WriteUninstaller "$INSTDIR\uninst-${PLUGIN_DLL}.exe"
    CreateDirectory "$SMPROGRAMS\Winamp\Vis Plugin Uninstallers"
    CreateShortCut "$SMPROGRAMS\Winamp\Vis Plugin Uninstallers\Uninstall ${PLUGIN_NAME}.lnk" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" "" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" 0
    goto UninstDone
    
  UninstDone:

SectionEnd

;----------------------------------------------------------------------

UninstallText "This will uninstall the ${PLUGIN_NAME} plugin.  Hit next to continue."
ShowUninstDetails Show

Section "Uninstall"

  ; This section is the code that will be run when the user goes
  ; to Uninstall the plugin.

  IfFileExists $INSTDIR\Winamp.exe UninstStep1Winamp2
  IfFileExists $INSTDIR\Winamp3.exe UninstStep1Winamp3
  IfFileExists $INSTDIR\Studio.exe UninstStep1Winamp3  ; legacy check
  goto UninstScriptDone

  UninstStep1Winamp3:
    ReadRegStr $1 HKLM SOFTWARE\${PLUGIN_DLL}Winamp3 "Install_Plugins_Dir"
    goto UninstStep2
  UninstStep1Winamp2:
    ReadRegStr $1 HKLM SOFTWARE\${PLUGIN_DLL}Winamp "Install_Plugins_Dir"
    goto UninstStep2

  UninstStep2:
  
    ; -------------------- EDIT THIS SECTION [3/3] --------------------
    ; -------------------- EDIT THIS SECTION [3/3] --------------------
    ; -------------------- EDIT THIS SECTION [3/3] --------------------

    ; LIST OF FILES TO DELETE WHEN USER RUNS THE UNINSTALL

    Delete "$1\${PLUGIN_DLL}"
    ;Delete "$1\vms_desktop.dll"     ** DO NOT DELETE! **
    Delete "$1\milkdrop.html"
    Delete "$1\milkdrop_preset_authoring.html"
    Delete "$1\q_and_t_vars.gif"

    MessageBox MB_YESNO|MB_ICONQUESTION \
            "Clear your saved settings?:$\r     $1\milkdrop_config.ini$\r     $1\milk_msg.ini$\r     $1\milk_img.ini" \
            IDNO SaveSettings

    Delete "$1\milkdrop_config.ini"
    Delete "$1\milk_msg.ini"
    Delete "$1\milk_img.ini"

    SaveSettings:

    MessageBox MB_YESNO|MB_ICONQUESTION \
            "Delete all presets in these 3 directories?:$\r     $1\milkdrop$\r     $1\milkdrop\3d$\r     $1\milkdrop\inverted" \
            IDNO SavePresets
            
    Delete "$1\milkdrop2\inverted\*.milk"
    Delete "$1\milkdrop2\3d\*.milk"
    Delete "$1\milkdrop2\*.milk"
    RMDir  "$1\milkdrop2\inverted"
    RMDir  "$1\milkdrop2\3d"
    RMDir  "$1\milkdrop2"
    
    SavePresets:
      
    ; ----------------------- END SECTION [3/3] -----------------------
    ; ----------------------- END SECTION [3/3] -----------------------
    ; ----------------------- END SECTION [3/3] -----------------------
  
  IfFileExists $INSTDIR\Winamp.exe UninstStep3Winamp2
  IfFileExists $INSTDIR\Winamp3.exe UninstStep3Winamp3
  IfFileExists $INSTDIR\Studio.exe UninstStep3Winamp3  ; legacy check
  goto UninstScriptDone
    
  UninstStep3Winamp3:
    DeleteRegKey HKLM SOFTWARE\${PLUGIN_DLL}Winamp3
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp3"
    Delete $INSTDIR\uninst-${PLUGIN_DLL}.exe
    Delete "$SMPROGRAMS\Winamp3\Vis Plugin Uninstallers\Uninstall ${PLUGIN_NAME}.lnk"
    RMDir "$SMPROGRAMS\Winamp3\Vis Plugin Uninstallers"
    goto UninstScriptDone
  UninstStep3Winamp2:
    DeleteRegKey HKLM SOFTWARE\${PLUGIN_DLL}Winamp
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp"
    Delete $INSTDIR\uninst-${PLUGIN_DLL}.exe
    Delete "$SMPROGRAMS\Winamp\Vis Plugin Uninstallers\Uninstall ${PLUGIN_NAME}.lnk"
    RMDir "$SMPROGRAMS\Winamp\Vis Plugin Uninstallers"
    goto UninstScriptDone

  UninstScriptDone:
SectionEnd

;----------------------------------------------------------------------

Function .onInstSuccess
  IfFileExists $INSTDIR\Winamp.exe CaseWinamp2
  IfFileExists $INSTDIR\Winamp3.exe CaseWinamp3
  IfFileExists $INSTDIR\Studio.exe CaseWinamp3  ; legacy check
  goto CaseImpossible

  CaseWinamp3:
    IfFileExists $INSTDIR\wacs\classicvis.wac ClassicVisOk
    ; no classicvis -> give them instructions
    MessageBox MB_YESNO|MB_ICONEXCLAMATION \
            "IMPORTANT: You must download and install the Classic Visualization$\rComponent before ${PLUGIN_NAME} will work with Winamp 3.$\r$\rWould you like to download it now?" \
            IDNO FailNoClassicVis
    ExecShell "open" "http://www.winamp.com/components3/detail.jhtml?componentId=122130"
    MessageBox MB_OK|MB_ICONINFORMATION \
            "Your web browser will now open and allow you to download$\rthe Classic Visualization Component.  Please download$\rand install it.$\r$\rOnce it is installed, open Winamp 3 and hit CTRL+P$\rto open the Preferences screen.  Then, on the left,$\rscroll to 'Classic Visualizations' and select it.  From there,$\ryou can easily select, configure and run any plugins$\rinstalled to Winamp 2 or 3."
    goto end            
  FailNoClassicVis:
    MessageBox MB_OK|MB_ICONSTOP "Installation failed."
    goto end            
  ClassicVisOk:
    FindWindow $R0 "STUDIO"
    StrCmp $R0 0 "" DoneWinamp3
    MessageBox MB_YESNO|MB_ICONQUESTION \
            "${PLUGIN_NAME} was installed successfully.$\rWould you like to run Winamp 3 now?" \
            IDNO DoneWinamp3
    ; to do here: update 'oldvisname' string in the *xml* file winamp3\studio.xnf,
    ; and set 'oldvisidx' to "0"
    IfFileExists $INSTDIR\Winamp3.exe CaseWinamp3b
      Exec '"$INSTDIR\studio.exe"'
      goto DoneWinamp3
    CaseWinamp3b:
      Exec '"$INSTDIR\winamp3.exe"'
      goto DoneWinamp3
  DoneWinamp3:
    MessageBox MB_OK "While in Winamp 3, press CTRL+P to bring up the$\rPreferences screen, then scroll down and select$\rthe 'Classic Visualizations' option.  From there,$\ryou can select, configure, and run the ${PLUGIN_NAME} plugin."
    goto end
      
  CaseWinamp2:
    ; note: winamp 2 should already be closed at this point.
    WriteINIStr "$INSTDIR\Winamp.ini" "Winamp" "visplugin_name" ${PLUGIN_DLL}
    WriteINIStr "$INSTDIR\Winamp.ini" "Winamp" "visplugin_num" "0"
    MessageBox MB_YESNO|MB_ICONQUESTION \
            "${PLUGIN_NAME} was installed successfully.$\r$\rWhile in Winamp, press ALT+K to configure it (optional);$\rpress CTRL+SHIFT+K to execute it.$\r$\rWould you like to run Winamp now?" \
            IDNO end
        Exec '"$INSTDIR\Winamp.exe"'
    Goto end
  
  CaseImpossible:  
    MessageBox MB_OK|MB_ICONEXCLAMATION "ERROR: unable to find winamp.exe (winamp2) or studio.exe/winamp3.exe (winamp3) in the install directory..."
    Goto end
    
  End:
FunctionEnd

; eof

