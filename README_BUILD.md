# Build Scripts

This project includes several build scripts to make building and running the project easier.

## Quick Start

### Build Only
```batch
# Build both Debug and Release (default)
build

# Build only Release
build release

# Build only Debug
build debug

# Clean and build both configurations
build clean

# Clean and build specific configuration
build clean release
build clean debug
```

### Build and Run
```batch
# Build and run Release version (default)
build_and_run

# Build and run Debug version
build_and_run debug

# Clean, build and run Release
build_and_run clean

# Clean, build and run Debug
build_and_run clean debug
```

## Script Details

### PowerShell Scripts
- `build.ps1`: Builds the project in specified configuration(s)
- `build_and_run.ps1`: Builds and runs the project in specified configuration

### Batch Files (Wrappers)
- `build.bat`: Easy-to-use wrapper for build.ps1
- `build_and_run.bat`: Easy-to-use wrapper for build_and_run.ps1

## Features
- Incremental builds by default (faster builds)
- Optional clean build with 'clean' argument
- CMake configuration and build
- Color-coded output
- Error checking at each step
- Shows executable location after build
- Reports program exit code when running

## Build Modes
1. Incremental Build (Default)
   - Reuses existing build files
   - Only rebuilds changed files
   - Faster build times
   - CMake reconfiguration skipped if build exists

2. Clean Build (with 'clean' argument)
   - Removes all build artifacts
   - Full CMake reconfiguration
   - Rebuilds everything from scratch
   - Use when build is corrupted or after major changes

## Requirements
- CMake 3.15 or higher
- Visual Studio 2022
- PowerShell

## Notes
- Scripts automatically bypass PowerShell execution policy
- Build artifacts are placed in the `build` directory
- Executables are placed in `build/bin/[Debug|Release]`
- Arguments are case-insensitive (debug/DEBUG/Debug all work) 