# Microsoft Developer Studio Project File - Name="plugin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plugin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plugin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plugin.mak" CFG="plugin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plugin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plugin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plugin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "plugin_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\SDKs\DirectX_9_Oct_2004\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx9.lib d3d9.lib Delayimp.lib /nologo /dll /machine:I386 /out:"c:\program files\winamp\plugins\vis_milk2.dll" /libpath:"..\SDKs\DirectX_9_Oct_2004\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "plugin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\SDKs\DirectX_9_Oct_2004\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx9.lib d3d9.lib delayimp.lib /nologo /dll /debug /machine:I386 /out:"c:\program files\winamp\plugins\vis_milk2.dll" /pdbtype:sept /libpath:"..\SDKs\DirectX_9_Oct_2004\lib"

!ENDIF 

# Begin Target

# Name "plugin - Win32 Release"
# Name "plugin - Win32 Debug"
# Begin Group "My Plugin Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\milkdropfs.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_icon.ico
# End Source File
# Begin Source File

SOURCE=.\state.cpp
# End Source File
# Begin Source File

SOURCE=.\support.cpp
# End Source File
# Begin Source File

SOURCE=.\texmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\textmgr.cpp
# End Source File
# End Group
# Begin Group "My Plugin Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\md_defines.h
# End Source File
# Begin Source File

SOURCE=.\menu.h
# End Source File
# Begin Source File

SOURCE=.\plugin.h
# End Source File
# Begin Source File

SOURCE=.\state.h
# End Source File
# Begin Source File

SOURCE=.\support.h
# End Source File
# Begin Source File

SOURCE=.\texmgr.h
# End Source File
# Begin Source File

SOURCE=.\textmgr.h
# End Source File
# End Group
# Begin Group "Framework Files (do not edit)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\config.cpp
# End Source File
# Begin Source File

SOURCE=.\config2.cpp
# End Source File
# Begin Source File

SOURCE=.\desktop_mode.cpp
# End Source File
# Begin Source File

SOURCE=.\dxcontext.cpp
# End Source File
# Begin Source File

SOURCE=.\dxcontext.h
# End Source File
# Begin Source File

SOURCE=.\fft.cpp
# End Source File
# Begin Source File

SOURCE=.\fft.h
# End Source File
# Begin Source File

SOURCE=.\gstring.h
# End Source File
# Begin Source File

SOURCE=.\icon_t.h
# End Source File
# Begin Source File

SOURCE=.\plugin.rc
# End Source File
# Begin Source File

SOURCE=.\pluginshell.cpp
# End Source File
# Begin Source File

SOURCE=.\pluginshell.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\shell_defines.h
# End Source File
# Begin Source File

SOURCE=.\utility.cpp
# End Source File
# Begin Source File

SOURCE=.\utility.h
# End Source File
# Begin Source File

SOURCE=.\vis.cpp
# End Source File
# Begin Source File

SOURCE=.\vis.h
# End Source File
# End Group
# Begin Group "evallib"

# PROP Default_Filter "*.c;*.h"
# Begin Source File

SOURCE=.\evallib\CAL_TAB.C
# End Source File
# Begin Source File

SOURCE=.\evallib\cal_tab.h
# End Source File
# Begin Source File

SOURCE=.\evallib\cfunc.c
# End Source File
# Begin Source File

SOURCE=.\evallib\Compiler.c
# End Source File
# Begin Source File

SOURCE=.\evallib\Compiler.h
# End Source File
# Begin Source File

SOURCE=.\evallib\eval.c
# End Source File
# Begin Source File

SOURCE=.\evallib\eval.h
# End Source File
# Begin Source File

SOURCE=.\evallib\Gettok.c
# End Source File
# Begin Source File

SOURCE=.\evallib\LEX.H
# End Source File
# Begin Source File

SOURCE=.\evallib\Lextab.c
# End Source File
# Begin Source File

SOURCE=.\evallib\LLSAVE.C
# End Source File
# Begin Source File

SOURCE=.\evallib\Yylex.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\DOCUMENTATION.TXT
# End Source File
# Begin Source File

SOURCE=.\milkdrop.nsi
# End Source File
# Begin Source File

SOURCE=.\temp.ico
# End Source File
# End Target
# End Project
