#include "configPanel.h"
#include <SDL.h>
#include <filesystem>
#include <imgui.h>
#include <iostream>

namespace module::editor
{
void ConfigPanel::render()
{
    if (ImGui::Begin("Configuration"))
    {
        auto &config = core::Config::get();
        bool needsSave = false;

        if (ImGui::CollapsingHeader("Window", ImGuiTreeNodeFlags_DefaultOpen))
        {
            renderWindowConfig();
            if (isDirty)
                needsSave = true;
        }

        if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            renderGraphicsConfig();
            if (isDirty)
                needsSave = true;
        }

        if (needsSave)
        {
            if (ImGui::Button("Save Changes"))
            {
                // Get the executable path
                char *basePath = SDL_GetBasePath();
                if (!basePath)
                {
                    std::cerr << "Failed to get base path: " << SDL_GetError() << std::endl;
                    return;
                }

                std::filesystem::path configPath = std::filesystem::path(basePath) / "config.json";
                SDL_free(basePath);

                if (config.saveToFile(configPath.string()))
                {
                    isDirty = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Revert Changes"))
            {
                char *basePath = SDL_GetBasePath();
                if (!basePath)
                {
                    std::cerr << "Failed to get base path: " << SDL_GetError() << std::endl;
                    return;
                }

                std::filesystem::path configPath = std::filesystem::path(basePath) / "config.json";
                SDL_free(basePath);

                if (config.loadFromFile(configPath.string()))
                {
                    isDirty = false;
                }
            }
        }
    }
    ImGui::End();
}

void ConfigPanel::renderWindowConfig()
{
    auto &config = core::Config::get();
    auto windowConfig = config.getWindowConfig();
    bool changed = false;

    // Title
    char title[256];
    strcpy_s(title, windowConfig.title.c_str());
    if (ImGui::InputText("Window Title", title, sizeof(title)))
    {
        windowConfig.title = title;
        changed = true;
    }

    // Dimensions
    int dimensions[2] = {windowConfig.width, windowConfig.height};
    if (ImGui::InputInt2("Window Size", dimensions))
    {
        windowConfig.width = dimensions[0];
        windowConfig.height = dimensions[1];
        changed = true;
    }

    // Fullscreen
    bool fullscreen = windowConfig.fullscreen;
    if (ImGui::Checkbox("Fullscreen", &fullscreen))
    {
        windowConfig.fullscreen = fullscreen;
        changed = true;
    }

    // VSync
    bool vsync = windowConfig.vsync;
    if (ImGui::Checkbox("VSync", &vsync))
    {
        windowConfig.vsync = vsync;
        changed = true;
    }

    if (changed)
    {
        config.setWindowConfig(windowConfig);
        isDirty = true;
    }
}

void ConfigPanel::renderGraphicsConfig()
{
    auto &config = core::Config::get();
    auto graphicsConfig = config.getGraphicsConfig();
    bool changed = false;

    // OpenGL Version
    int version[2] = {graphicsConfig.glMajorVersion, graphicsConfig.glMinorVersion};
    if (ImGui::InputInt2("OpenGL Version", version))
    {
        graphicsConfig.glMajorVersion = version[0];
        graphicsConfig.glMinorVersion = version[1];
        changed = true;
    }

    // MSAA Samples
    int samples = graphicsConfig.msaaSamples;
    if (ImGui::InputInt("MSAA Samples", &samples))
    {
        graphicsConfig.msaaSamples = samples;
        changed = true;
    }

    // Debug Output
    bool debug = graphicsConfig.enableDebugOutput;
    if (ImGui::Checkbox("Debug Output", &debug))
    {
        graphicsConfig.enableDebugOutput = debug;
        changed = true;
    }

    if (changed)
    {
        config.setGraphicsConfig(graphicsConfig);
        isDirty = true;
    }
}
} // namespace module::editor