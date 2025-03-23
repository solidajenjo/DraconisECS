# Building DraconisECS

## Prerequisites

### Required Software
- Visual Studio 2022 with C++ Desktop Development workload
- CMake 3.15 or higher (for initial dependency builds)
- Premake5 (for Visual Studio solution generation)
- Git (for cloning the repository)
- Ninja build system (included with Visual Studio 2022)

### Dependencies
The following dependencies are included in the repository under the `ThirdParty` directory:
- SDL2 2.30.8 (built automatically during compilation)
- GLEW 2.2.0
- Dear ImGui (latest)
- nlohmann/json (downloaded automatically during CMake configuration)

## Building the Project

### Step 1: Build Dependencies with CMake

First, you need to build the dependencies using CMake. This step is required before using Premake5.

```bash
scripts\build.bat            # Debug build (default)
scripts\build.bat debug      # Debug build explicitly
scripts\build.bat release    # Release build
```

This will:
1. Build SDL2 from source
2. Set up all necessary dependencies
3. Create the required DLLs and libraries

### Step 2: Generate Visual Studio Solution with Premake5

After the dependencies are built, you can generate a Visual Studio solution:

1. Install Premake5:
   - Download from https://premake.github.io/download/
   - Add Premake5 to your system PATH

2. Generate Visual Studio Solution:
   ```bash
   premake5 vs2022
   ```

3. Open the generated solution:
   - Navigate to the `.vs` directory
   - Open `DraconisECS.sln`
   - Select your desired configuration (Debug, Release, or Dist)
   - Build the solution

The executable and required DLLs will be placed in the `bin/[Configuration]/DraconisECS` directory.

### Alternative: Using CMake for Development

If you prefer using CMake directly for development:

```bash
scripts\build.bat            # Debug build (default)
scripts\build.bat debug      # Debug build explicitly
scripts\build.bat release    # Release build
scripts\build.bat clean      # Clean project-specific build files (preserves SDL2)
scripts\build.bat clean all  # Clean all build files including SDL2
```

The executable and required DLLs will be placed in the `build/bin` directory.

> **Note:** All commands are case-insensitive. For example, `debug`, `DEBUG`, and `Debug` are all valid.

## Running the Project

After building, you can run the application using the run script from any location:
```bash
scripts\run.bat            # Run release build (default)
scripts\run.bat debug      # Run debug build
scripts\run.bat release    # Run release build explicitly
```

The script will automatically:
- Build the project if needed
- Check if the build exists
- Verify the executable and DLLs are present
- Run from the correct directory
- Return any error codes from the application

## Project Structure
```
DraconisECS/
├── .vs/                # Visual Studio solution files (Premake5)
├── build/             # CMake build output directory
│   └── bin/          # Binaries directory
│       ├── Debug/    # Debug configuration output
│       └── Release/  # Release configuration output
├── bin/              # Premake5 build output directory
│   ├── Debug/       # Debug configuration output
│   ├── Release/     # Release configuration output
│   └── Dist/        # Distribution configuration output
├── bin-int/         # Premake5 intermediate files
├── DraconisECS/     # Source code
├── scripts/         # Build and run scripts
├── ThirdParty/      # Third-party dependencies
├── config.json      # Application configuration
├── layout.json      # UI layout configuration
├── imgui.ini        # ImGui configuration
├── CMakeLists.txt   # CMake build configuration
└── premake5.lua     # Premake5 build configuration
```

## Troubleshooting

### Common Issues

1. **Missing Visual Studio Components**
   - Error: "Could not find Visual Studio x64 environment setup script"
   - Solution: Install/repair Visual Studio 2022 with the "Desktop development with C++" workload

2. **GLEW DLL Missing**
   - Error: "The code execution cannot proceed because glew32.dll was not found"
   - Solution: Ensure `glew32.dll` is present in `ThirdParty/glew-2.2.0/bin/Release/x64/`

3. **SDL2 Build Fails**
   - Error: "Error copying SDL2d.dll"
   - Solution: 
     - Ensure you have write permissions in the build directory
     - Try running `.\scripts\build.bat clean` to clean project files
     - If the issue persists, try `.\scripts\build.bat clean all` to rebuild SDL2 as well

4. **Configuration Files Missing**
   - Error: "Failed to load config.json"
   - Solution: Ensure `config.json`, `layout.json`, and `imgui.ini` exist in the root directory

### Building from Visual Studio

You can build the project in Visual Studio using either method:

#### Using Premake5 (Recommended)
1. Ensure dependencies are built using CMake first
2. Generate the solution: `premake5 vs2022`
3. Open `DraconisECS.sln` from the `.vs` directory
4. Select your desired configuration (Debug, Release, or Dist)
5. Build -> Build Solution

#### Using CMake
1. Open Visual Studio 2022
2. File -> Open -> CMake -> Select the `CMakeLists.txt` in the root directory
3. Select "x64-Debug" or "x64-Release" configuration
4. Build -> Build All

## Additional Notes

- The project uses C++17 features
- Exception handling is disabled by default
- Debug builds include full debugging information
- The application requires OpenGL 4.3 or higher
- All configuration files are automatically copied to the build output directory

## Compiler Flags

### Debug Build
For MSVC (Visual Studio):
- `/Zi` - Generate complete debugging information
- `/Od` - Disable optimizations
- `/DEBUG` - Create debugging information
- `/EHs-c-` - Disable C++ exception handling
- `/W4` - Warning level 4 (high)
- `/MDd` - Use debug multithreaded DLL runtime
- `/we4189` - Treat unused local variables as errors

For GCC/Clang:
- `-g` - Generate debugging information
- `-O0` - Disable optimizations
- `-fno-exceptions` - Disable exception handling
- `-Wall -Wextra` - Enable comprehensive warnings
- `-Werror=unused-variable` - Treat unused variables as errors

### Release Build
For MSVC (Visual Studio):
- `/EHs-c-` - Disable C++ exception handling
- `/W4` - Warning level 4 (high)
- `/MD` - Use release multithreaded DLL runtime
- `/we4189` - Treat unused local variables as errors
- Default optimization flags

For GCC/Clang:
- `-fno-exceptions` - Disable exception handling
- `-Wall -Wextra` - Enable comprehensive warnings
- `-Werror=unused-variable` - Treat unused variables as errors
- Default optimization flags

## Support

For issues and questions:
- Create an issue in the repository
- Check the existing issues for similar problems
- Include build logs and system information when reporting problems
