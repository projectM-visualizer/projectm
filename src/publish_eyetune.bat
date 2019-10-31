REM @echo off
@echo on
Echo Windows SYNC 

REM # XEON x64 Build Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set INITDIR=%CD%
SET GITPath="C:\Program Files\Git\bin"
set PATH=%GITPath%;%PATH%
set REPO=%_SCRIPT_FOLDER%


REM # VC Vars #
SET tbs_arch=x64
SET vcvar_arg=x86_amd64
SET VCVAR="%programfiles(x86)%\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%
SET VCVAR="%programfiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%


bash %REPO%publish_eyetune.sh

pause
