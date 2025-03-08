#include "window.h"
#include "core/config.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>

namespace module
{
bool Window::init()
{
    // Load configuration
    auto &config = core::Config::get();
    if (!config.loadFromFile("config.json"))
    {
        std::cerr << "Failed to load config file, using defaults" << std::endl;
    }

    const auto &windowConfig = config.getWindowConfig();
    const auto &graphicsConfig = config.getGraphicsConfig();

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, graphicsConfig.glMajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, graphicsConfig.glMinorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    if (graphicsConfig.msaaSamples > 0)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, graphicsConfig.msaaSamples);
    }

    // Create window
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (windowConfig.fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window = SDL_CreateWindow(windowConfig.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              windowConfig.width, windowConfig.height, flags);

    if (!window)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set VSync
    if (SDL_GL_SetSwapInterval(windowConfig.vsync ? 1 : 0) < 0)
    {
        std::cerr << "Warning: Unable to set VSync! SDL_Error: " << SDL_GetError() << std::endl;
    }

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
