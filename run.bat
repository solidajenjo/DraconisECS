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

:: Copy DLL files if they don't exist
if not exist "build\bin\%CONFIG%\SDL2.dll" (
    echo Copying SDL2.dll to %CONFIG% directory...
    copy "build\bin\Release\SDL2.dll" "build\bin\%CONFIG%\SDL2.dll"
)
if not exist "build\bin\%CONFIG%\glew32.dll" (
    echo Copying glew32.dll to %CONFIG% directory...
    copy "build\bin\Release\glew32.dll" "build\bin\%CONFIG%\glew32.dll"
)

start "" "build\bin\%CONFIG%\DraconisECS.exe"
goto end

:error
echo Build failed!
pause
exit /b 1

:end
echo Build and launch successful!
exit /b 0 