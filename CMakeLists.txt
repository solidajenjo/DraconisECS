cmake_minimum_required(VERSION 3.15)
project(DraconisECS VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable exceptions and set debug flags based on compiler
if(MSVC)
    # Disable all exception handling consistently
    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(/EHs-c-)
    add_definitions(-D_HAS_EXCEPTIONS=0)
    
    # Add debug info in Debug mode
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od /DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
    
    # Use debug runtime library in Debug mode
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
else()
    add_compile_options(-fno-exceptions)
    add_definitions(-DBOOST_NO_EXCEPTIONS)
    
    # Add debug symbols for non-MSVC compilers
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
endif()

# Set build type
set(CMAKE_BUILD_TYPE Debug)

# Set paths for third-party libraries
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL2-2.30.8")
set(GLEW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glew-2.2.0")
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui")
set(JSON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/json")

# Configure SDL2 build options
set(SDL_SHARED ON CACHE BOOL "Build SDL2 as a shared library")
set(SDL_STATIC OFF CACHE BOOL "Build SDL2 as a static library")
set(SDL_TEST OFF CACHE BOOL "Build SDL2 tests")

# Set SDL2 output directories to match our project structure
set(SDL_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/SDL2_build" CACHE PATH "Where to place SDL2 DLLs")
set(SDL_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/SDL2_build" CACHE PATH "Where to place SDL2 libraries")
set(SDL_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/SDL2_build" CACHE PATH "Where to place SDL2 archives")

# Add SDL2 as a subdirectory project
add_subdirectory("${SDL2_DIR}" "SDL2_build" EXCLUDE_FROM_ALL)

# Create nlohmann directory and download JSON library if not exists
file(MAKE_DIRECTORY "${JSON_DIR}/nlohmann")
if(NOT EXISTS "${JSON_DIR}/nlohmann/json.hpp")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
        "${JSON_DIR}/nlohmann/json.hpp"
        SHOW_PROGRESS
    )
endif()

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
    ${JSON_DIR}
)

# Link directories
target_link_directories(${PROJECT_NAME} PRIVATE
    ${GLEW_DIR}/lib/Release/x64
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    glew32
    opengl32
)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
)

# Add compile options based on compiler
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif() 