# Building DraconisECS

## Prerequisites

### Required Software
- Visual Studio 2022 with C++ Desktop Development workload
- CMake 3.15 or higher
- Git (for cloning the repository)
- Ninja build system (included with Visual Studio 2022)

### Dependencies
The following dependencies are included in the repository under the `ThirdParty` directory:
- SDL2 2.30.8 (built automatically during compilation)
- GLEW 2.2.0
- Dear ImGui (latest)
- nlohmann/json (downloaded automatically during CMake configuration)

## Building the Project

The project uses the Ninja build system for faster builds and better reliability. Ninja is included with Visual Studio 2022's CMake tools.

Simply run the build script from the project root directory:
```bash
.\build.bat
```

The executable and required DLLs will be placed in the `build/bin` directory.

### Build Configurations
- Debug build (default): `.\build.bat debug`
- Release build: `.\build.bat release`
- Clean build files: `.\build.bat clean`

## Running the Project

After building, you can run the application using the run script:
```bash
.\run.bat            # Run release build (default)
.\run.bat debug      # Run debug build
.\run.bat release    # Run release build explicitly
```

The script will automatically:
- Check if the build exists
- Verify the executable is present
- Run from the correct directory
- Return any error codes from the application

## Project Structure
```
DraconisECS/
├── build/              # Build output directory
│   └── bin/           # Binaries directory
│       ├── Debug/     # Debug configuration output
│       └── Release/   # Release configuration output
├── DraconisECS/       # Source code
├── ThirdParty/        # Third-party dependencies
├── config.json        # Application configuration
├── layout.json        # UI layout configuration
└── imgui.ini          # ImGui configuration
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
     - Try running `.\build.bat clean` before rebuilding

4. **Configuration Files Missing**
   - Error: "Failed to load config.json"
   - Solution: Ensure `config.json`, `layout.json`, and `imgui.ini` exist in the root directory

### Building from Visual Studio

If you prefer using Visual Studio directly:
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
