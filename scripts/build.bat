@echo off
setlocal EnableDelayedExpansion

:: Store the script directory and change to project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
cd /d "%PROJECT_ROOT%"

:: Set default configuration to Debug
set CONFIG=Debug
set BUILD_DIR=%PROJECT_ROOT%\build

:: Parse command line arguments (case-insensitive)
if "%~1"=="" (
    echo No configuration specified, using default: %CONFIG%
) else (
    if /I "%~1"=="release" (
        set CONFIG=Release
    ) else if /I "%~1"=="debug" (
        set CONFIG=Debug
    ) else if /I "%~1"=="clean" if /I "%~2"=="all" (
        if exist "%BUILD_DIR%" (
            echo Cleaning all build files including SDL2...
            rd /s /q "%BUILD_DIR%"
            echo All build files cleaned.
        ) else (
            echo Build directory does not exist.
        )
        exit /b 0
    ) else if /I "%~1"=="clean" (
        echo Cleaning project-specific build files...
        :: Delete only project-specific files and directories
        if exist "%BUILD_DIR%\bin" rd /s /q "%BUILD_DIR%\bin"
        if exist "%BUILD_DIR%\CMakeFiles" rd /s /q "%BUILD_DIR%\CMakeFiles"
        if exist "%BUILD_DIR%\DraconisECS.dir" rd /s /q "%BUILD_DIR%\DraconisECS.dir"
        if exist "%BUILD_DIR%\x64" rd /s /q "%BUILD_DIR%\x64"
        del /f /q "%BUILD_DIR%\*.cmake" 2>nul
        del /f /q "%BUILD_DIR%\CMakeCache.txt" 2>nul
        del /f /q "%BUILD_DIR%\*.ninja" 2>nul
        echo Project-specific build files cleaned.
        exit /b 0
    ) else (
        echo Error: Unsupported option '%~1'
        echo Supported options are:
        echo   debug      - Build in Debug configuration
        echo   release    - Build in Release configuration
        echo   clean      - Clean project-specific build files ^(preserves SDL2^)
        echo   clean all  - Clean all build files including SDL2
        echo   [none]     - Default to Debug configuration
        exit /b 1
    )
)

echo Building with configuration: %CONFIG%

:: Find Visual Studio installation using more specific criteria
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set VS_PATH=%%i
)

:: Check if VS_PATH exists and contains vcvars64.bat
if not exist "!VS_PATH!\VC\Auxiliary\Build\vcvars64.bat" (
    echo Error: Could not find Visual Studio x64 environment setup script
    echo Please install Visual Studio with C++ development tools
    exit /b 1
)

echo Found Visual Studio at !VS_PATH!

:: Create a temporary batch file to run build commands
set BUILD_SCRIPT=%TEMP%\build_%RANDOM%.bat
echo @echo off > "%BUILD_SCRIPT%"
echo call "!VS_PATH!\VC\Auxiliary\Build\vcvars64.bat" >> "%BUILD_SCRIPT%"
echo set CC=cl.exe >> "%BUILD_SCRIPT%"
echo set CXX=cl.exe >> "%BUILD_SCRIPT%"
echo cmake -B "%BUILD_DIR%" -G "Ninja Multi-Config" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe >> "%BUILD_SCRIPT%"
echo cmake --build "%BUILD_DIR%" --config %CONFIG% >> "%BUILD_SCRIPT%"

:: Run the build script
call "%BUILD_SCRIPT%"
set BUILD_RESULT=%ERRORLEVEL%

:: Clean up the temporary script
del "%BUILD_SCRIPT%"

if %BUILD_RESULT% neq 0 (
    echo Build failed with error code %BUILD_RESULT%
    exit /b %BUILD_RESULT%
)

:: Create bin directory for the current configuration
set BIN_DIR=%BUILD_DIR%\bin\%CONFIG%
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

:: Wait for SDL2 build to complete and verify DLL exists
set SDL2_DLL_PATH=
if /I "%CONFIG%"=="Debug" (
    set SDL2_DLL_PATH=%BUILD_DIR%\SDL2_build\Debug\SDL2d.dll
) else (
    set SDL2_DLL_PATH=%BUILD_DIR%\SDL2_build\Release\SDL2.dll
)

:: Check if SDL2 DLL exists
if not exist "!SDL2_DLL_PATH!" (
    echo Error: SDL2 DLL not found at !SDL2_DLL_PATH!
    echo Checking alternative locations...
    
    if /I "%CONFIG%"=="Debug" (
        if exist "%BUILD_DIR%\SDL2_build\SDL2d.dll" (
            set SDL2_DLL_PATH=%BUILD_DIR%\SDL2_build\SDL2d.dll
        )
    ) else (
        if exist "%BUILD_DIR%\SDL2_build\SDL2.dll" (
            set SDL2_DLL_PATH=%BUILD_DIR%\SDL2_build\SDL2.dll
        )
    )
    
    if not exist "!SDL2_DLL_PATH!" (
        echo Error: Could not find SDL2 DLL in any expected location
        echo This might indicate that SDL2 failed to build properly
        echo Expected locations:
        echo   - %BUILD_DIR%\SDL2_build\%CONFIG%\SDL2.dll
        echo   - %BUILD_DIR%\SDL2_build\SDL2.dll
        exit /b 1
    )
)

:: Copy SDL2 DLL
echo Copying SDL2 DLL from !SDL2_DLL_PATH!...
copy /Y "!SDL2_DLL_PATH!" "%BIN_DIR%\" || (
    echo Error: Failed to copy SDL2 DLL
    exit /b 1
)

:: Check if GLEW DLL exists
if not exist "%PROJECT_ROOT%\ThirdParty\glew-2.2.0\bin\Release\x64\glew32.dll" (
    echo Error: GLEW DLL not found in ThirdParty\glew-2.2.0\bin\Release\x64\
    exit /b 1
)

:: Copy GLEW DLL
echo Copying glew32.dll...
copy /Y "%PROJECT_ROOT%\ThirdParty\glew-2.2.0\bin\Release\x64\glew32.dll" "%BIN_DIR%\" || (
    echo Error: Failed to copy GLEW DLL
    exit /b 1
)

:: Copy configuration files if they don't exist in the destination
echo Copying configuration files...
for %%f in (config.json layout.json imgui.ini) do (
    if exist "%%f" (
        if not exist "%BIN_DIR%\%%f" copy /Y "%%f" "%BIN_DIR%\%%f" || (
            echo Warning: Failed to copy %%f
        )
    ) else (
        echo Warning: %%f not found in root directory
    )
)

echo Build completed successfully.
exit /b 0 