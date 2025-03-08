param(
    [Parameter()]
    [ValidateSet('Release', 'Debug', 'Both')]
    [string]$Configuration = 'Both',
    
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
    Write-Host "Executable location: $PWD\build\bin\$BuildType\DraconisECS.exe`n"
}

try {
    # Clean build directory only if -Clean switch is used
    if ($Clean -and (Test-Path build)) {
        Write-Host "Cleaning build directory..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force build
    }

    switch ($Configuration) {
        'Release' {
            Build-Project 'Release'
        }
        'Debug' {
            Build-Project 'Debug'
        }
        'Both' {
            Build-Project 'Debug'
            Build-Project 'Release'
        }
    }

    Write-Host "All builds completed successfully!" -ForegroundColor Green
} catch {
    Write-Host "An error occurred: $_" -ForegroundColor Red
    exit 1
} 