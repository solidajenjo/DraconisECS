param(
    [Parameter()]
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    
    [Parameter()]
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

function Build-Project {
    param (
        [string]$BuildType
    )
    # Only configure CMake if build directory doesn't exist
    if (-not (Test-Path build)) {
        Write-Host "`nConfiguring CMake for $BuildType..." -ForegroundColor Cyan
        cmake -B build -G "Visual Studio 17 2022" -A x64
        if ($LASTEXITCODE -ne 0) {
            Write-Host "CMake configuration failed!" -ForegroundColor Red
            exit 1
        }
    }

    Write-Host "`nBuilding $BuildType configuration..." -ForegroundColor Cyan
    cmake --build build --config $BuildType
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed for $BuildType configuration!" -ForegroundColor Red
        exit 1
    }
    
    Write-Host "`nBuild successful for $BuildType configuration!" -ForegroundColor Green
    $exePath = "$PWD\build\bin\$BuildType\DraconisECS.exe"
    Write-Host "Executable location: $exePath`n"
    return $exePath
}

try {
    # Clean build directory only if -Clean switch is used
    if ($Clean -and (Test-Path build)) {
        Write-Host "Cleaning build directory..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force build
    }

    # Build the project
    $exePath = Build-Project $Configuration

    # Check if executable exists
    if (-not (Test-Path $exePath)) {
        Write-Host "Error: Executable not found at $exePath" -ForegroundColor Red
        exit 1
    }

    # Run the executable
    Write-Host "`nRunning $Configuration build..." -ForegroundColor Cyan
    Write-Host "----------------------------------------`n" -ForegroundColor White
    
    Push-Location (Split-Path $exePath)
    & $exePath
    $exitCode = $LASTEXITCODE
    Pop-Location
    
    Write-Host "`n----------------------------------------" -ForegroundColor White

    # Check exit code
    if ($exitCode -eq 0) {
        Write-Host "Program exited successfully (Exit code: 0)" -ForegroundColor Green
    } else {
        Write-Host "Program exited with code: $exitCode" -ForegroundColor Yellow
    }
} catch {
    Write-Host "An error occurred: $_" -ForegroundColor Red
    exit 1
} 