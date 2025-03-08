cmake_minimum_required(VERSION 3.15)
project(DraconisECS VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set paths for third-party libraries
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL2-2.30.8")
set(GLEW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glew-2.2.0")
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui")

# Add ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Find all source files
file(GLOB_RECURSE SOURCES 
    "DraconisECS/*.cpp"
    "DraconisECS/core/*.cpp"
    "DraconisECS/core/*.h"
    "DraconisECS/modules/*.cpp"
    "DraconisECS/modules/*.h"
)

# Add main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/DraconisECS
    ${CMAKE_CURRENT_SOURCE_DIR}/DraconisECS/core
    ${CMAKE_CURRENT_SOURCE_DIR}/DraconisECS/modules
    ${SDL2_DIR}/include
    ${GLEW_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Link directories
target_link_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_DIR}/build/Release
    ${GLEW_DIR}/lib/Release/x64
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2
    SDL2main
    glew32
    opengl32
)

# Copy SDL2 and GLEW DLLs to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DIR}/build/Release/SDL2.dll"
        "${CMAKE_BINARY_DIR}/bin/Release/SDL2.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLEW_DIR}/bin/Release/x64/glew32.dll"
        "${CMAKE_BINARY_DIR}/bin/Release/glew32.dll"
)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add compile options based on compiler
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif() 