@echo off
setlocal

set CONFIG=Both
set CLEAN=

:parse_args
if "%1"=="" goto execute
if /i "%1"=="clean" (
    set CLEAN=-Clean
    shift
    goto parse_args
)
if /i "%1"=="debug" (
    set CONFIG=Debug
    shift
    goto parse_args
)
if /i "%1"=="release" (
    set CONFIG=Release
    shift
    goto parse_args
)
if /i "%1"=="both" (
    set CONFIG=Both
    shift
    goto parse_args
)

:execute
powershell -ExecutionPolicy Bypass -File "%~dp0build.ps1" -Configuration %CONFIG% %CLEAN% 