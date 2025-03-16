@echo off
setlocal EnableDelayedExpansion

:: Store the script directory and change to project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
cd /d "%PROJECT_ROOT%"

echo ========================================
echo Starting build system validation tests
echo ========================================

set FAILED_TESTS=0
set TOTAL_TESTS=0

:: Function to run test and check result
call :run_test "Clean all build files" "call %SCRIPT_DIR%build.bat clean all"

:: Test debug build
call :run_test "Debug build" "call %SCRIPT_DIR%build.bat debug"
call :run_test "Debug build executable exists" "if exist build\bin\Debug\DraconisECS.exe (exit /b 0) else (exit /b 1)"
call :run_test "Debug SDL2 DLL exists" "if exist build\bin\Debug\SDL2d.dll (exit /b 0) else (exit /b 1)"
call :run_test "Debug GLEW DLL exists" "if exist build\bin\Debug\glew32.dll (exit /b 0) else (exit /b 1)"

:: Test release build
call :run_test "Release build" "call %SCRIPT_DIR%build.bat release"
call :run_test "Release build executable exists" "if exist build\bin\Release\DraconisECS.exe (exit /b 0) else (exit /b 1)"
call :run_test "Release SDL2 DLL exists" "if exist build\bin\Release\SDL2.dll (exit /b 0) else (exit /b 1)"
call :run_test "Release GLEW DLL exists" "if exist build\bin\Release\glew32.dll (exit /b 0) else (exit /b 1)"

:: Test clean (preserving SDL2)
call :run_test "Clean project files" "call %SCRIPT_DIR%build.bat clean"
call :run_test "SDL2 build files preserved" "if exist build\SDL2_build (exit /b 0) else (exit /b 1)"
call :run_test "Project files cleaned" "if not exist build\bin (exit /b 0) else (exit /b 1)"

:: Test clean all
call :run_test "Clean all files" "call %SCRIPT_DIR%build.bat clean all"
call :run_test "All files cleaned" "if not exist build (exit /b 0) else (exit /b 1)"

:: Test rebuild after clean
call :run_test "Rebuild after clean" "call %SCRIPT_DIR%build.bat debug"

:: Skip running the executable in CI environment
if "%CI_BUILD%"=="1" (
    echo Skipping executable tests in CI environment...
    goto :skip_run_tests
)

:: Test run script with debug
call :run_test_with_timeout "Run debug build" "%SCRIPT_DIR%run.bat debug" "DraconisECS.exe" 5

:: Test run script with release
call :run_test_with_timeout "Run release build" "%SCRIPT_DIR%run.bat release" "DraconisECS.exe" 5

:skip_run_tests

:: Test invalid options
call :run_test "Invalid build option" "call %SCRIPT_DIR%build.bat invalid && exit /b 1 || exit /b 0"
call :run_test "Invalid run option" "call %SCRIPT_DIR%run.bat invalid && exit /b 1 || exit /b 0"

echo ========================================
echo Test Summary:
echo Tests run: %TOTAL_TESTS%
echo Tests failed: %FAILED_TESTS%
echo ========================================

if %FAILED_TESTS% gtr 0 (
    echo Some tests failed! Check the output above for details.
    exit /b 1
) else (
    echo All tests passed successfully!
    exit /b 0
)

:: Test runner function
:run_test
setlocal
set "TEST_NAME=%~1"
set "TEST_CMD=%~2"
set /a TOTAL_TESTS+=1
echo.
echo Testing: %TEST_NAME%
echo Command: %TEST_CMD%
%TEST_CMD%
if %ERRORLEVEL% neq 0 (
    echo [FAILED] %TEST_NAME%
    set /a FAILED_TESTS+=1
) else (
    echo [PASSED] %TEST_NAME%
)
echo.
endlocal & set TOTAL_TESTS=%TOTAL_TESTS% & set FAILED_TESTS=%FAILED_TESTS%
exit /b

:: Test runner function with timeout
:run_test_with_timeout
setlocal
set "TEST_NAME=%~1"
set "TEST_CMD=%~2"
set "PROCESS_NAME=%~3"
set "TIMEOUT_SECONDS=%~4"
set /a TOTAL_TESTS+=1
echo.
echo Testing: %TEST_NAME%
echo Command: %TEST_CMD%

:: Start the process in the background
start "" cmd /c %TEST_CMD%

:: Wait for the process to start (give it a moment)
timeout /t 2 /nobreak > nul

:: Check if the process is running
tasklist /FI "IMAGENAME eq %PROCESS_NAME%" 2>NUL | find /I /N "%PROCESS_NAME%" >NUL
if %ERRORLEVEL% neq 0 (
    echo [FAILED] %TEST_NAME% - Process failed to start
    set /a FAILED_TESTS+=1
    goto :test_with_timeout_end
)

:: Wait for the specified time
timeout /t %TIMEOUT_SECONDS% /nobreak > nul

:: Kill the process and its child processes
for /f "tokens=2" %%a in ('tasklist /fi "imagename eq %PROCESS_NAME%" /fo list ^| find "PID:"') do (
    taskkill /F /T /PID %%a >nul 2>&1
)

:: Verify the process was killed
timeout /t 1 /nobreak > nul
tasklist /FI "IMAGENAME eq %PROCESS_NAME%" 2>NUL | find /I /N "%PROCESS_NAME%" >NUL
if %ERRORLEVEL% equ 0 (
    echo [FAILED] %TEST_NAME% - Process could not be terminated
    set /a FAILED_TESTS+=1
) else (
    echo [PASSED] %TEST_NAME%
)

:test_with_timeout_end
echo.
endlocal & set TOTAL_TESTS=%TOTAL_TESTS% & set FAILED_TESTS=%FAILED_TESTS%
exit /b 