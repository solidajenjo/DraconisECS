@echo off
setlocal EnableDelayedExpansion

:: Set default configuration to Release
set CONFIG=Release

:: Parse command line arguments
if "%1"=="debug" (
    set CONFIG=Debug
)

:: Store the original directory
set ORIGINAL_DIR=%CD%

:: Set paths
set BUILD_DIR=%ORIGINAL_DIR%\build
set BIN_DIR=%BUILD_DIR%\bin\%CONFIG%
set EXE_PATH=%BIN_DIR%\DraconisECS.exe

echo Building %CONFIG% configuration...
call build.bat %1
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
if "%CONFIG%"=="Debug" (
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

:: Verify bin directory exists and is accessible
if not exist "%BIN_DIR%" (
    echo Error: Binary directory %BIN_DIR% does not exist
    exit /b 1
)


:: Try to change to the binary directory
cd /d "%BIN_DIR%" || (
    echo Error: Failed to change to directory %BIN_DIR%
    cd /d "%ORIGINAL_DIR%"
    exit /b 1
)

echo Executing: "%EXE_PATH%"

:: Run the executable
"%EXE_PATH%"
set RUN_RESULT=%ERRORLEVEL%

:: Return to original directory
cd /d "%ORIGINAL_DIR%" || (
    echo Warning: Failed to return to original directory %ORIGINAL_DIR%
)

if %RUN_RESULT% neq 0 (
    echo Program exited with error code %RUN_RESULT%
    exit /b %RUN_RESULT%
)

exit /b 0 