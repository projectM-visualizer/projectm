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

bash %REPO%_sync.sh

