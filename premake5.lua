workspace "DraconisECS"
    architecture "x64"
    startproject "DraconisECS"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

project "DraconisECS"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/%{cfg.buildcfg}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/%{cfg.buildcfg}/%{prj.name}")

    pchheader "pch.h"
    pchsource "DraconisECS/pch.cpp"

    files
    {
        "DraconisECS/**.h",
        "DraconisECS/**.cpp",
        "DraconisECS/core/**.h",
        "DraconisECS/core/**.cpp",
        "DraconisECS/modules/**.h",
        "DraconisECS/modules/**.cpp"
    }

    includedirs
    {
        "DraconisECS",
        "DraconisECS/core",
        "DraconisECS/modules",
        "DraconisECS/config",
        "ThirdParty/SDL2-2.30.8/include",
        "ThirdParty/glew-2.2.0/include",
        "ThirdParty/imgui",
        "ThirdParty/imgui/backends",
        "ThirdParty/json"
    }

    libdirs
    {
        "ThirdParty/glew-2.2.0/lib/Release/x64"
    }

    links
    {
        "SDL2",
        "SDL2main",
        "glew32",
        "opengl32"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines "DRA_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "DRA_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "DRA_DIST"
        runtime "Release"
        optimize "On" 