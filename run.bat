@echo off
setlocal

:: Default to Release if no configuration specified
set CONFIG=Release
if /i "%1"=="debug" set CONFIG=Debug
if /i "%1"=="release" set CONFIG=Release

echo Building %CONFIG% configuration...

cmake -B build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 goto error

cmake --build build --config %CONFIG%
if errorlevel 1 goto error

start "" "build\bin\%CONFIG%\DraconisECS.exe"
goto end

:error
echo Build failed!
pause
exit /b 1

:end
echo Build and launch successful!
exit /b 0 