@echo off
setlocal EnableDelayedExpansion

:: Store the script directory and change to project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
cd /d "%PROJECT_ROOT%"

:: Set default configuration to Release
set CONFIG=Release

:: Parse command line arguments (case-insensitive)
if not "%~1"=="" (
    if /I "%~1"=="debug" (
        set CONFIG=Debug
    ) else if /I "%~1"=="release" (
        set CONFIG=Release
    ) else (
        echo Error: Unsupported option '%~1'
        echo Supported options are:
        echo   debug    - Build and run in Debug configuration
        echo   release  - Build and run in Release configuration
        echo   [none]   - Default to Release configuration
        exit /b 1
    )
)

:: Set paths relative to project root
set BUILD_DIR=%PROJECT_ROOT%\build
set BIN_DIR=%BUILD_DIR%\bin\%CONFIG%
set EXE_PATH=%BIN_DIR%\DraconisECS.exe

echo Building %CONFIG% configuration...
:: Pass the configuration in lowercase to build.bat
if /I "%CONFIG%"=="Debug" (
    call "%SCRIPT_DIR%build.bat" debug
) else (
    call "%SCRIPT_DIR%build.bat" release
)

if %ERRORLEVEL% neq 0 (
    echo Build failed with error code %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

:: Check if executable exists
if not exist "%EXE_PATH%" (
    echo Error: DraconisECS.exe not found in %BIN_DIR%
    echo Build appears to have failed. Please check the build output above.
    exit /b 1
)

:: Verify all required DLLs are present
set MISSING_DEPS=0

:: Check SDL2
if /I "%CONFIG%"=="Debug" (
    if not exist "%BIN_DIR%\SDL2d.dll" (
        echo Error: SDL2d.dll is missing from %BIN_DIR%
        set MISSING_DEPS=1
    )
) else (
    if not exist "%BIN_DIR%\SDL2.dll" (
        echo Error: SDL2.dll is missing from %BIN_DIR%
        set MISSING_DEPS=1
    )
)

:: Check GLEW
if not exist "%BIN_DIR%\glew32.dll" (
    echo Error: glew32.dll is missing from %BIN_DIR%
    set MISSING_DEPS=1
)

:: Check configuration files
for %%f in (config.json layout.json imgui.ini) do (
    if not exist "%BIN_DIR%\%%f" (
        echo Warning: %%f is missing from %BIN_DIR%
    )
)

if %MISSING_DEPS% neq 0 (
    echo One or more required DLLs are missing. Please rebuild the project.
    exit /b 1
)

:: Change to the binary directory and run the executable
cd /d "%BIN_DIR%" || (
    echo Error: Failed to change to directory %BIN_DIR%
    exit /b 1
)

echo Executing: "%EXE_PATH%"
"%EXE_PATH%"
set RUN_RESULT=%ERRORLEVEL%

if %RUN_RESULT% neq 0 (
    echo Program exited with error code %RUN_RESULT%
    exit /b %RUN_RESULT%
)

exit /b 0 