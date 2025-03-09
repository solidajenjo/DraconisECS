#include "editor.h"
#include "app.h"
#include "core/editorPanels/configPanel.h"
#include "core/editorPanels/renderPanel.h"
#include "core/editorPanels/stylePanel.h"
#include "filesystem.h"
#include "window.h"
#include <SDL.h>
#include <fstream>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace module
{
bool Editor::init()
{
    // Get window module
    Window &windowModule = app::appInstance.getModule<Window>();
    SDL_Window *window = windowModule.getWindow();
    SDL_GLContext glContext = SDL_GL_GetCurrentContext();

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Set default theme
    setDefaultTheme();

    // Add default menu items
    addMenuItem("File", "Save Layout", [this]() { saveLayout(); });
    addMenuItem("File", "Load Layout", [this]() { loadLayout(); });
    addMenuItem("File", "Reset Layout", [this]() { resetLayout(); });

    // Add editor panels
    addPanel<editor::ConfigPanel>();
    addPanel<editor::StylePanel>();

    // Add render panel with render module dependency
    Render &renderModule = app::appInstance.getModule<Render>();
    addPanel<editor::RenderPanel>(&renderModule);

    // Try to load saved layout
    if (!loadLayout())
    {
        // If loading fails, we already have the default theme from setDefaultTheme()
        return true;
    }

    return true;
}

bool Editor::update()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Apply current theme
    applyTheme();

    // Create a dockspace over the entire viewport
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr,
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground);

    ImGui::PopStyleVar(3);

    // Add menu bar
    renderMenuBar();

    // DockSpace
    ImGui::DockSpace(ImGui::GetID("MyDockSpace"));
    ImGui::End();
    // Render all visible panels
    for (const auto &[name, panel] : panels)
    {
        if (panelStates[name].isVisible)
        {
            panel->render();
        }
    }

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return true;
}

bool Editor::shutdown()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}

bool Editor::removePanel(const std::string &panelName)
{
    auto it = panels.find(panelName);
    if (it != panels.end())
    {
        panels.erase(it);
        panelStates.erase(panelName);
        return true;
    }
    return false;
}

bool Editor::showPanel(const std::string &panelName, bool show)
{
    auto it = panelStates.find(panelName);
    if (it != panelStates.end())
    {
        it->second.isVisible = show;
        return true;
    }
    return false;
}

bool Editor::isPanelVisible(const std::string &panelName) const
{
    auto it = panelStates.find(panelName);
    return it != panelStates.end() && it->second.isVisible;
}

std::vector<std::string> Editor::getActivePanels() const
{
    std::vector<std::string> activePanels;
    for (const auto &[name, state] : panelStates)
    {
        if (state.isVisible)
        {
            activePanels.push_back(name);
        }
    }
    return activePanels;
}

bool Editor::saveLayout(const std::string &filename) const
{
    // Get the executable path
    char *basePath = SDL_GetBasePath();
    if (!basePath)
    {
        std::cerr << "Failed to get base path: " << SDL_GetError() << std::endl;
        return false;
    }

    std::filesystem::path layoutPath = std::filesystem::path(basePath) / (filename.empty() ? "layout.json" : filename);
    SDL_free(basePath);

    json j;

    // Save panel states
    for (const auto &[name, state] : panelStates)
    {
        j["panels"][name] = {
            {"visible", state.isVisible},
            {"docked", state.isDocked},
            {"position", {state.position.x, state.position.y}},
            {"size", {state.size.x, state.size.y}},
            {"dockId", static_cast<uint32_t>(state.dockId)} // Save as uint32_t
        };
    }

    // Save complete theme
    j["theme"] = {
        {"textColor",
         {currentTheme.textColor.x, currentTheme.textColor.y, currentTheme.textColor.z, currentTheme.textColor.w}},
        {"bgColor", {currentTheme.bgColor.x, currentTheme.bgColor.y, currentTheme.bgColor.z, currentTheme.bgColor.w}},
        {"titlebarColor",
         {currentTheme.titlebarColor.x, currentTheme.titlebarColor.y, currentTheme.titlebarColor.z,
          currentTheme.titlebarColor.w}},
        {"titlebarActiveColor",
         {currentTheme.titlebarActiveColor.x, currentTheme.titlebarActiveColor.y, currentTheme.titlebarActiveColor.z,
          currentTheme.titlebarActiveColor.w}},
        {"accentColor",
         {currentTheme.accentColor.x, currentTheme.accentColor.y, currentTheme.accentColor.z,
          currentTheme.accentColor.w}},
        {"accentActiveColor",
         {currentTheme.accentActiveColor.x, currentTheme.accentActiveColor.y, currentTheme.accentActiveColor.z,
          currentTheme.accentActiveColor.w}},
        {"buttonColor",
         {currentTheme.buttonColor.x, currentTheme.buttonColor.y, currentTheme.buttonColor.z,
          currentTheme.buttonColor.w}},
        {"buttonHoverColor",
         {currentTheme.buttonHoverColor.x, currentTheme.buttonHoverColor.y, currentTheme.buttonHoverColor.z,
          currentTheme.buttonHoverColor.w}},
        {"buttonActiveColor",
         {currentTheme.buttonActiveColor.x, currentTheme.buttonActiveColor.y, currentTheme.buttonActiveColor.z,
          currentTheme.buttonActiveColor.w}},
        {"rounding", currentTheme.rounding},
        {"borderSize", currentTheme.borderSize}};

    return Filesystem::writeJsonFile(layoutPath.string(), j);
}

bool Editor::loadLayout(const std::string &filename)
{
    // Get the executable path
    char *basePath = SDL_GetBasePath();
    if (!basePath)
    {
        std::cerr << "Failed to get base path: " << SDL_GetError() << std::endl;
        return false;
    }

    std::filesystem::path layoutPath = std::filesystem::path(basePath) / (filename.empty() ? "layout.json" : filename);
    SDL_free(basePath);

    json j;
    if (!Filesystem::readJsonFile(layoutPath.string(), j))
    {
        return false;
    }

    // Load panel states
    if (j.contains("panels"))
    {
        for (auto &[name, panel] : j["panels"].items())
        {
            if (panelStates.find(name) != panelStates.end())
            {
                auto &state = panelStates[name];
                if (panel.contains("visible"))
                    state.isVisible = panel["visible"].get<bool>();
                if (panel.contains("docked"))
                    state.isDocked = panel["docked"].get<bool>();
                if (panel.contains("position") && panel["position"].size() == 2)
                {
                    state.position = ImVec2(panel["position"][0].get<float>(), panel["position"][1].get<float>());
                }
                if (panel.contains("size") && panel["size"].size() == 2)
                {
                    state.size = ImVec2(panel["size"][0].get<float>(), panel["size"][1].get<float>());
                }
                if (panel.contains("dockId"))
                {
                    // Load as uint32_t and cast to ImGuiID
                    state.dockId = static_cast<ImGuiID>(panel["dockId"].get<uint32_t>());
                }
            }
        }
    }

    // Load theme
    if (j.contains("theme"))
    {
        const auto &themeData = j["theme"];
        editor::EditorTheme theme = currentTheme;

        auto loadColor = [](const json &data, ImVec4 &color) {
            if (data.size() == 4)
            {
                color.x = data[0].get<float>();
                color.y = data[1].get<float>();
                color.z = data[2].get<float>();
                color.w = data[3].get<float>();
            }
        };

        if (themeData.contains("textColor"))
            loadColor(themeData["textColor"], theme.textColor);
        if (themeData.contains("bgColor"))
            loadColor(themeData["bgColor"], theme.bgColor);
        if (themeData.contains("titlebarColor"))
            loadColor(themeData["titlebarColor"], theme.titlebarColor);
        if (themeData.contains("titlebarActiveColor"))
            loadColor(themeData["titlebarActiveColor"], theme.titlebarActiveColor);
        if (themeData.contains("accentColor"))
            loadColor(themeData["accentColor"], theme.accentColor);
        if (themeData.contains("accentActiveColor"))
            loadColor(themeData["accentActiveColor"], theme.accentActiveColor);
        if (themeData.contains("buttonColor"))
            loadColor(themeData["buttonColor"], theme.buttonColor);
        if (themeData.contains("buttonHoverColor"))
            loadColor(themeData["buttonHoverColor"], theme.buttonHoverColor);
        if (themeData.contains("buttonActiveColor"))
            loadColor(themeData["buttonActiveColor"], theme.buttonActiveColor);
        if (themeData.contains("rounding"))
            theme.rounding = themeData["rounding"].get<float>();
        if (themeData.contains("borderSize"))
            theme.borderSize = themeData["borderSize"].get<float>();

        setTheme(theme);
    }

    return true;
}

void Editor::resetLayout()
{
    // Reset panel states
    for (auto &[name, state] : panelStates)
    {
        state = editor::PanelState{};
    }

    // Reset theme
    setDefaultTheme();
}

void Editor::setTheme(const editor::EditorTheme &theme)
{
    currentTheme = theme;
    applyTheme();
}

void Editor::setDefaultTheme()
{
    editor::EditorTheme theme;

    // Light theme colors
    theme.textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    theme.bgColor = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    theme.titlebarColor = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    theme.titlebarActiveColor = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
    theme.accentColor = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    theme.accentActiveColor = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
    theme.buttonColor = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    theme.buttonHoverColor = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
    theme.buttonActiveColor = ImVec4(0.71f, 0.71f, 0.71f, 1.0f);

    theme.rounding = 3.0f;
    theme.borderSize = 1.0f;

    setTheme(theme);
}

void Editor::addMenuItem(const std::string &menu, const std::string &item, std::function<void()> callback)
{
    editor::MenuItem menuItem{item, callback};
    menuItems[menu].push_back(menuItem);
}

void Editor::addSubMenuItem(const std::string &menu, const std::string &item, const std::string &subItem,
                            std::function<void()> callback)
{
    for (auto &menuItem : menuItems[menu])
    {
        if (menuItem.name == item)
        {
            menuItem.subItems.push_back({subItem, callback});
            return;
        }
    }

    // If parent item doesn't exist, create it and add the sub-item
    editor::MenuItem parent{item};
    parent.subItems.push_back({subItem, callback});
    menuItems[menu].push_back(parent);
}

void Editor::renderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        for (const auto &[menuName, items] : menuItems)
        {
            if (ImGui::BeginMenu(menuName.c_str()))
            {
                for (const auto &item : items)
                {
                    if (item.subItems.empty())
                    {
                        if (ImGui::MenuItem(item.name.c_str()))
                        {
                            item.callback();
                        }
                    }
                    else
                    {
                        if (ImGui::BeginMenu(item.name.c_str()))
                        {
                            for (const auto &subItem : item.subItems)
                            {
                                if (ImGui::MenuItem(subItem.name.c_str()))
                                {
                                    subItem.callback();
                                }
                            }
                            ImGui::EndMenu();
                        }
                    }
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenuBar();
    }
}

void Editor::applyTheme() const
{
    ImGuiStyle &style = ImGui::GetStyle();

    // Colors
    auto &colors = style.Colors;
    colors[ImGuiCol_Text] = currentTheme.textColor;
    colors[ImGuiCol_WindowBg] = currentTheme.bgColor;
    colors[ImGuiCol_TitleBg] = currentTheme.titlebarColor;
    colors[ImGuiCol_TitleBgActive] = currentTheme.titlebarActiveColor;
    colors[ImGuiCol_Button] = currentTheme.buttonColor;
    colors[ImGuiCol_ButtonHovered] = currentTheme.buttonHoverColor;
    colors[ImGuiCol_ButtonActive] = currentTheme.buttonActiveColor;

    // Accent colors for various interactive elements
    colors[ImGuiCol_HeaderHovered] = currentTheme.accentColor;
    colors[ImGuiCol_HeaderActive] = currentTheme.accentActiveColor;
    colors[ImGuiCol_TabHovered] = currentTheme.accentColor;
    colors[ImGuiCol_TabActive] = currentTheme.accentActiveColor;
    colors[ImGuiCol_TabUnfocusedActive] = currentTheme.accentColor;

    // Style
    style.WindowRounding = currentTheme.rounding;
    style.FrameRounding = currentTheme.rounding;
    style.PopupRounding = currentTheme.rounding;
    style.ScrollbarRounding = currentTheme.rounding;
    style.GrabRounding = currentTheme.rounding;
    style.TabRounding = currentTheme.rounding;

    style.WindowBorderSize = currentTheme.borderSize;
    style.FrameBorderSize = currentTheme.borderSize;
    style.PopupBorderSize = currentTheme.borderSize;
}

} // namespace module
