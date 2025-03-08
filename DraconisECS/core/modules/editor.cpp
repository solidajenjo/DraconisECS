#include "editor.h"
#include "SDL.h"
#include "app.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "editorPanels/renderEP.h"
#include "imgui.h"
#include "modules/window.h"

namespace module
{
static constexpr const char *GLSL_VERSION = "#version 130";

bool Editor::init()
{
    SDL_Window *window = app::appInstance.getModule<module::Window>().getWindow();
    if (!window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window handle for editor initialization");
        return false;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create ImGui context");
        return false;
    }

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
    // ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    if (!ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext()))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize ImGui SDL2 backend");
        ImGui::DestroyContext();
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init(GLSL_VERSION))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize ImGui OpenGL3 backend");
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        return false;
    }

    try
    {
        addPanel<editorPanel::RenderEP>();
    }
    catch (const std::exception &e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create editor panel: %s", e.what());
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        return false;
    }

    return true;
}

bool Editor::update()
{
    // draw imgui test window
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // render all editor panels
    for (const auto &panel : panels)
    {
        panel->render();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window *backupCurrentWindow = SDL_GL_GetCurrentWindow();
        SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);
    }
    return true;
}

bool Editor::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    // No need to manually delete panels - unique_ptr handles cleanup
    return true;
}
} // namespace module
