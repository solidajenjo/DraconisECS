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
set "BUILD_LOG=%TEMP%\build_log_%RANDOM%.txt"
set "ERROR_LOG=%TEMP%\error_log_%RANDOM%.txt"
set "TEST_LOG=%TEMP%\test_log_%RANDOM%.txt"

:: Initialize test log
type nul > "%TEST_LOG%"

:: Clean and build debug configuration
echo.
echo Testing: Clean and build debug
set /a TOTAL_TESTS+=1
call %SCRIPT_DIR%build.bat clean all > "%BUILD_LOG%" 2>&1
call %SCRIPT_DIR%build.bat debug >> "%BUILD_LOG%" 2>&1
if !ERRORLEVEL! neq 0 (
    echo [FAILED] Debug build
    powershell -Command "$errors = Get-Content '%BUILD_LOG%' | Select-String -Pattern 'error C[0-9]+:' -Context 0,1; $errors | ForEach-Object { $_.Line } | Out-File '%ERROR_LOG%' -Encoding ASCII"
    echo [Debug Build] Failed with exit code !ERRORLEVEL! >> "%TEST_LOG%"
    echo Error details: >> "%TEST_LOG%"
    type "%ERROR_LOG%" >> "%TEST_LOG%"
    echo ---------------------------------------- >> "%TEST_LOG%"
    set /a FAILED_TESTS+=1
) else (
    echo [PASSED] Debug build
)

:: Clean and build release configuration
echo.
echo Testing: Clean and build release
set /a TOTAL_TESTS+=1
call %SCRIPT_DIR%build.bat clean all > "%BUILD_LOG%" 2>&1
call %SCRIPT_DIR%build.bat release >> "%BUILD_LOG%" 2>&1
if !ERRORLEVEL! neq 0 (
    echo [FAILED] Release build
    powershell -Command "$errors = Get-Content '%BUILD_LOG%' | Select-String -Pattern 'error C[0-9]+:' -Context 0,1; $errors | ForEach-Object { $_.Line } | Out-File '%ERROR_LOG%' -Encoding ASCII"
    echo [Release Build] Failed with exit code !ERRORLEVEL! >> "%TEST_LOG%"
    echo Error details: >> "%TEST_LOG%"
    type "%ERROR_LOG%" >> "%TEST_LOG%"
    echo ---------------------------------------- >> "%TEST_LOG%"
    set /a FAILED_TESTS+=1
) else (
    echo [PASSED] Release build
)

:: Skip running the executable in CI environment
if "%CI_BUILD%"=="1" (
    echo Skipping executable tests in CI environment...
    goto :skip_run_tests
)

:: Run tests
call :run_executable_test "Debug" "debug" 10
call :run_executable_test "Release" "release" 15

:skip_run_tests

:: Display test summary
echo ========================================
echo Test Summary
echo ========================================
echo Total tests run: %TOTAL_TESTS%
echo Failed tests: %FAILED_TESTS%
if %FAILED_TESTS% gtr 0 (
    echo.
    echo Failed Tests Details:
    type "%TEST_LOG%"
)
echo ========================================

:: Clean up temporary files
del "%BUILD_LOG%" 2>nul
del "%ERROR_LOG%" 2>nul
del "%TEST_LOG%" 2>nul

exit /b %FAILED_TESTS%

:: Function to run executable tests
:run_executable_test
setlocal
set "BUILD_TYPE=%~1"
set "CONFIG=%~2"
set "TIMEOUT=%~3"
set /a TOTAL_TESTS+=1

echo.
echo Testing: Run %BUILD_TYPE% build
echo Command: %SCRIPT_DIR%run.bat %CONFIG%

:: Ensure no existing instances are running
call :terminate_process "DraconisECS.exe"
timeout /t 2 /nobreak > nul

:: Run the executable
start "" cmd /c %SCRIPT_DIR%run.bat %CONFIG% > "%BUILD_LOG%" 2>&1

:: Wait for process to start (up to 10 seconds)
set /a "WAIT_TIME=10"
:wait_loop
tasklist /FI "IMAGENAME eq DraconisECS.exe" 2>NUL | find /I /N "DraconisECS.exe" >NUL
if !ERRORLEVEL! neq 0 (
    timeout /t 1 /nobreak > nul
    set /a "WAIT_TIME-=1"
    if !WAIT_TIME! gtr 0 goto :wait_loop
    echo [Run %BUILD_TYPE%] Failed with exit code 1 >> "%TEST_LOG%"
    echo Error details: >> "%TEST_LOG%"
    echo Process failed to start within 10 seconds >> "%TEST_LOG%"
    echo ---------------------------------------- >> "%TEST_LOG%"
    set /a FAILED_TESTS+=1
    goto :run_test_end
)

:: Get the PID
for /f "tokens=2" %%a in ('tasklist /fi "imagename eq DraconisECS.exe" /fo list ^| find "PID:"') do set "PROCESS_PID=%%a"
echo Process started with PID: !PROCESS_PID!

:: Wait for specified timeout
timeout /t %TIMEOUT% /nobreak > nul

:: Terminate the process
call :terminate_process "DraconisECS.exe"
if !ERRORLEVEL! neq 0 (
    echo [Run %BUILD_TYPE%] Failed with exit code 1 >> "%TEST_LOG%"
    echo Error details: >> "%TEST_LOG%"
    echo Failed to terminate process (PID: !PROCESS_PID!) >> "%TEST_LOG%"
    echo ---------------------------------------- >> "%TEST_LOG%"
    set /a FAILED_TESTS+=1
) else (
    echo [PASSED] Run %BUILD_TYPE% build
)

:run_test_end
endlocal & set TOTAL_TESTS=%TOTAL_TESTS% & set FAILED_TESTS=%FAILED_TESTS%
exit /b

:: Enhanced process termination function
:terminate_process
setlocal
set "PROCESS_NAME=%~1"
set "MAX_ATTEMPTS=3"
set "CURRENT_ATTEMPT=1"

echo Attempting to terminate %PROCESS_NAME%...

:terminate_loop
tasklist /FI "IMAGENAME eq %PROCESS_NAME%" 2>NUL | find /I /N "%PROCESS_NAME%" >NUL
if !ERRORLEVEL! neq 0 (
    echo Process %PROCESS_NAME% is not running.
    exit /b 0
)

echo Attempt !CURRENT_ATTEMPT! of %MAX_ATTEMPTS%...

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
if !ERRORLEVEL! equ 0 (
    set /a CURRENT_ATTEMPT+=1
    if !CURRENT_ATTEMPT! leq %MAX_ATTEMPTS% (
        goto :terminate_loop
    ) else (
        echo Failed to terminate %PROCESS_NAME% after %MAX_ATTEMPTS% attempts.
        exit /b 1
    )
)

echo Successfully terminated %PROCESS_NAME%
exit /b 0

endlocal
exit /b 