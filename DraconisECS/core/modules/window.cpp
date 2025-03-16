#include "window.h"
#include "core/config.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>

namespace module
{
bool Window::init()
{
    std::cout << "Window::init() - Starting initialization..." << std::endl;

    // Load configuration
    auto &config = core::Config::get();
    if (!config.loadFromFile("config.json"))
    {
        std::cerr << "Failed to load config file, using defaults" << std::endl;
    }
    else
    {
        std::cout << "Successfully loaded config.json" << std::endl;
    }

    const auto &windowConfig = config.getWi_ndowConfig();
    const auto &graphicsConfig = config.getGraphicsConfig();

    std::cout << "Window config - Title: " << windowConfig.title << ", Size: " << windowConfig.width << "x"
              << windowConfig.height << ", Fullscreen: " << (windowConfig.fullscreen ? "yes" : "no")
              << ", VSync: " << (windowConfig.vsync ? "yes" : "no") << std::endl;

    std::cout << "Graphics config - OpenGL: " << graphicsConfig.glMajorVersion << "." << graphicsConfig.glMinorVersion
              << ", MSAA: " << graphicsConfig.msaaSamples << "x" << std::endl;

    // Initialize SDL
    std::cout << "Initializing SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL initialized successfully" << std::endl;

    // Set OpenGL attributes
    std::cout << "Setting OpenGL attributes..." << std::endl;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, graphicsConfig.glMajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, graphicsConfig.glMinorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    if (graphicsConfig.msaaSamples > 0)
    {
        std::cout << "Enabling MSAA with " << graphicsConfig.msaaSamples << " samples" << std::endl;
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, graphicsConfig.msaaSamples);
    }

    // Create window
    std::cout << "Creating window..." << std::endl;
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (windowConfig.fullscreen)
    {
        std::cout << "Enabling fullscreen mode" << std::endl;
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow(windowConfig.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              windowConfig.width, windowConfig.height, flags);

    if (!window)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "Window created successfully" << std::endl;

    // Create OpenGL context
    std::cout << "Creating OpenGL context..." << std::endl;
    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        // Print available OpenGL versions
        SDL_GL_ResetAttributes();
        for (int major = 4; major >= 2; major--)
        {
            for (int minor = 6; minor >= 0; minor--)
            {
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
                if (SDL_GL_CreateContext(window))
                {
                    std::cout << "Available OpenGL version: " << major << "." << minor << std::endl;
                    SDL_GL_DeleteContext(glContext);
                }
            }
        }
        return false;
    }
    std::cout << "OpenGL context created successfully" << std::endl;

    // Set VSync
    std::cout << "Setting VSync..." << std::endl;
    if (SDL_GL_SetSwapInterval(windowConfig.vsync ? 1 : 0) < 0)
    {
        std::cerr << "Warning: Unable to set VSync! SDL_Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        std::cout << "VSync set successfully" << std::endl;
    }

    // Print OpenGL information
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;

    std::cout << "Window::init() - Initialization complete" << std::endl;
    return true;
}

bool Window::postUpdate()
{
    SDL_GL_SwapWindow(window);
    return true;
}

bool Window::shutdown()
{
    if (glContext)
    {
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
    return true;
}
} // namespace module
