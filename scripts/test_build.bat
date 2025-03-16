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

:: Ensure no existing instances are running
call :terminate_process "DraconisECS.exe"
timeout /t 2 /nobreak > nul

:: Test run script with debug
call :run_test_with_timeout "Run debug build" "%SCRIPT_DIR%run.bat debug" "DraconisECS.exe" 10

:: Ensure cleanup between tests
call :terminate_process "DraconisECS.exe"
timeout /t 2 /nobreak > nul

:: Rebuild release to ensure it's fresh
call :run_test "Rebuild release" "call %SCRIPT_DIR%build.bat release"
timeout /t 2 /nobreak > nul

:: Test run script with release
call :run_test_with_timeout "Run release build" "%SCRIPT_DIR%run.bat release" "DraconisECS.exe" 15

:: Final cleanup
call :terminate_process "DraconisECS.exe"

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

:: Enhanced process termination function with multiple attempts
:terminate_process
setlocal
set "PROCESS_NAME=%~1"
set "MAX_ATTEMPTS=3"
set "CURRENT_ATTEMPT=1"

echo Attempting to terminate %PROCESS_NAME%...

:terminate_loop
tasklist /FI "IMAGENAME eq %PROCESS_NAME%" 2>NUL | find /I /N "%PROCESS_NAME%" >NUL
if %ERRORLEVEL% neq 0 (
    echo Process %PROCESS_NAME% is not running.
    goto :terminate_success
)

echo Attempt %CURRENT_ATTEMPT% of %MAX_ATTEMPTS%...

:: Get all PIDs for the process
for /f "tokens=2" %%a in ('tasklist /fi "imagename eq %PROCESS_NAME%" /fo list ^| find "PID:"') do (
    echo Found %PROCESS_NAME% with PID: %%a
    
    :: Try graceful termination first
    echo Attempting graceful termination of PID %%a...
    taskkill /PID %%a >nul 2>&1
    
    :: Wait briefly
    timeout /t 1 /nobreak > nul
    
    :: Check if process still exists
    tasklist /FI "PID eq %%a" 2>NUL | find /I /N "%%a" >NUL
    if !ERRORLEVEL! equ 0 (
        :: Force termination if graceful attempt failed
        echo Forcing termination of PID %%a...
        taskkill /F /T /PID %%a >nul 2>&1
    )
)

:: Wait and verify
timeout /t 2 /nobreak > nul

:: Check if process is still running
tasklist /FI "IMAGENAME eq %PROCESS_NAME%" 2>NUL | find /I /N "%PROCESS_NAME%" >NUL
if %ERRORLEVEL% equ 0 (
    set /a CURRENT_ATTEMPT+=1
    if %CURRENT_ATTEMPT% leq %MAX_ATTEMPTS% (
        goto :terminate_loop
    ) else (
        echo Failed to terminate %PROCESS_NAME% after %MAX_ATTEMPTS% attempts.
        exit /b 1
    )
) else (
    goto :terminate_success
)

:terminate_success
echo Successfully terminated %PROCESS_NAME%
exit /b 0

endlocal
exit /b

:: Test runner function with timeout and enhanced process management
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

:: Wait for the process to start (up to 10 seconds)
echo Waiting for process to start...
set /a "WAIT_TIME=10"
:wait_loop
tasklist /FI "IMAGENAME eq %PROCESS_NAME%" 2>NUL | find /I /N "%PROCESS_NAME%" >NUL
if %ERRORLEVEL% neq 0 (
    timeout /t 1 /nobreak > nul
    set /a "WAIT_TIME-=1"
    if %WAIT_TIME% gtr 0 goto :wait_loop
    echo [FAILED] %TEST_NAME% - Process failed to start within 10 seconds
    set /a FAILED_TESTS+=1
    goto :test_with_timeout_end
)

:: Get the PID of the started process
for /f "tokens=2" %%a in ('tasklist /fi "imagename eq %PROCESS_NAME%" /fo list ^| find "PID:"') do (
    set "PROCESS_PID=%%a"
)
echo Process started with PID: !PROCESS_PID!

echo Process started successfully, waiting %TIMEOUT_SECONDS% seconds...
timeout /t %TIMEOUT_SECONDS% /nobreak > nul

:: Attempt to terminate the process
call :terminate_process "%PROCESS_NAME%"
if %ERRORLEVEL% neq 0 (
    echo [FAILED] %TEST_NAME% - Process could not be terminated
    set /a FAILED_TESTS+=1
) else (
    echo [PASSED] %TEST_NAME%
)

:test_with_timeout_end
echo.
endlocal & set TOTAL_TESTS=%TOTAL_TESTS% & set FAILED_TESTS=%FAILED_TESTS%
exit /b 