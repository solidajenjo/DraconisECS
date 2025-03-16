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
set WARNINGS=0
set TOTAL_TESTS=0
set "BUILD_LOG=%TEMP%\build_log_%RANDOM%.txt"
set "ERROR_LOG=%TEMP%\error_log_%RANDOM%.txt"
set "WARNING_LOG=%TEMP%\warning_log_%RANDOM%.txt"
set "TEST_LOG=%TEMP%\test_log_%RANDOM%.txt"
set "TEST_LOG_WARNING=%TEMP%\test_log_warning_%RANDOM%.txt"

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
    powershell -Command "$warnings = Get-Content '%BUILD_LOG%' | Select-String -Pattern 'warning C[0-9]+:' -Context 0,1; $warnings | ForEach-Object { $_.Line } | Out-File '%WARNING_LOG%' -Encoding ASCII"
    for /f %%i in ("%WARNING_LOG%") do set size=%%~zi
        if !size! GTR 0 (
            echo Debug Build Warning Details: >> "%TEST_LOG_WARNING%"
            echo ======================================== >> "%TEST_LOG_WARNING%"
            type "%WARNING_LOG%" >> "%TEST_LOG_WARNING%"
            echo ---------------------------------------- >> "%TEST_LOG_WARNING%"
            echo. >> "%TEST_LOG_WARNING%"
            set /a WARNINGS+=1
        )

    set /a FAILED_TESTS+=1
) else (
    powershell -Command "$warnings = Get-Content '%BUILD_LOG%' | Select-String -Pattern 'warning C[0-9]+:' -Context 0,1; $warnings | ForEach-Object { $_.Line } | Out-File '%WARNING_LOG%' -Encoding ASCII"
    for /f %%i in ("%WARNING_LOG%") do set size=%%~zi
        if !size! GTR 0 (
            echo [PASSED WITH WARNINGS] Debug build
            echo Debug Build Warning Details: >> "%TEST_LOG_WARNING%"
            echo ======================================== >> "%TEST_LOG_WARNING%"
            type "%WARNING_LOG%" >> "%TEST_LOG_WARNING%"
            echo ---------------------------------------- >> "%TEST_LOG_WARNING%"            
            echo. >> "%TEST_LOG_WARNING%"
            set /a WARNINGS+=1
        ) else (
            echo [PASSED] Debug build            
        )            
)
::clear warning log
type nul > "%WARNING_LOG%"
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
    powershell -Command "$warnings = Get-Content '%BUILD_LOG%' | Select-String -Pattern 'warning C[0-9]+:' -Context 0,1; $warnings | ForEach-Object { $_.Line } | Out-File '%WARNING_LOG%' -Encoding ASCII"
    for /f %%i in ("%WARNING_LOG%") do set size=%%~zi
        if !size! GTR 0 (
            echo Release Build Warning Details: >> "%TEST_LOG_WARNING%"
            echo ======================================== >> "%TEST_LOG_WARNING%"
            type "%WARNING_LOG%" >> "%TEST_LOG_WARNING%"
            echo ---------------------------------------- >> "%TEST_LOG_WARNING%"
            echo. >> "%TEST_LOG_WARNING%"
            set /a WARNINGS+=1
        )
    set /a FAILED_TESTS+=1
) else (
    powershell -Command "$warnings = Get-Content '%BUILD_LOG%' | Select-String -Pattern 'warning C[0-9]+:' -Context 0,1; $warnings | ForEach-Object { $_.Line } | Out-File '%WARNING_LOG%' -Encoding ASCII"
    for /f %%i in ("%WARNING_LOG%") do set size=%%~zi
        if !size! GTR 0 (
            echo [PASSED WITH WARNINGS] Release build
            echo Release Build Warning Details: >> "%TEST_LOG_WARNING%"
            echo ======================================== >> "%TEST_LOG_WARNING%"
            type "%WARNING_LOG%" >> "%TEST_LOG_WARNING%"
            echo ---------------------------------------- >> "%TEST_LOG_WARNING%"            
            echo. >> "%TEST_LOG_WARNING%"
            set /a WARNINGS+=1
        ) else (
            echo [PASSED] Release build            
        )  
)

:: Display test summary
echo.
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
echo.
::print warning log
if %WARNINGS% gtr 0 (
    echo ========================================
    echo Warning Details:   
    echo.
    type "%TEST_LOG_WARNING%"
    echo ========================================
    echo Total warnings: %WARNINGS%
)

:: Clean up temporary files
del "%BUILD_LOG%" 2>nul
del "%ERROR_LOG%" 2>nul
del "%WARNING_LOG%" 2>nul
del "%TEST_LOG%" 2>nul
del "%TEST_LOG_WARNING%" 2>nul

exit /b %FAILED_TESTS%

echo Successfully terminated %PROCESS_NAME%
exit /b 0

endlocal
exit /b 