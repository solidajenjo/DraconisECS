#pragma once
#include "Module.h"
#include <functional>
#include <imgui.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct SDL_Window;

namespace module
{
namespace editor
{
// Forward declarations
class EditorPanel;
struct PanelState;
struct EditorTheme;
struct MenuItem;

class EditorPanel
{
  public:
    virtual void render() = 0;
    virtual ~EditorPanel() = default;
    virtual const char *getName() const = 0;
};

struct PanelState
{
    bool isVisible = true;
    bool isDocked = false;
    ImVec2 position{0, 0};
    ImVec2 size{0, 0};
    std::string dockId;
};

struct EditorTheme
{
    ImVec4 textColor{0, 0, 0, 1};
    ImVec4 bgColor{0.94f, 0.94f, 0.94f, 1.0f};
    ImVec4 titlebarColor{0.85f, 0.85f, 0.85f, 1.0f};
    ImVec4 titlebarActiveColor{0.75f, 0.75f, 0.75f, 1.0f};
    ImVec4 accentColor{0.26f, 0.59f, 0.98f, 0.67f};
    ImVec4 accentActiveColor{0.26f, 0.59f, 0.98f, 1.0f};
    ImVec4 buttonColor{0.85f, 0.85f, 0.85f, 1.0f};
    ImVec4 buttonHoverColor{0.78f, 0.78f, 0.78f, 1.0f};
    ImVec4 buttonActiveColor{0.71f, 0.71f, 0.71f, 1.0f};
    float rounding = 3.0f;
    float borderSize = 1.0f;
};

struct MenuItem
{
    std::string name;
    std::function<void()> callback;
    std::vector<MenuItem> subItems;
};
} // namespace editor

class Editor : public Module
{
  public:
    bool init() override;
    bool update() override;
    bool shutdown() override;

    // Panel Management
    template <typename Panel> void addPanel()
    {
        static_assert(std::is_base_of_v<editor::EditorPanel, Panel>, "Panel must inherit from EditorPanel");
        auto panel = std::make_unique<Panel>();
        const char *name = panel->getName();
        panels[name] = std::move(panel);
        panelStates[name] = editor::PanelState{};
    }

    bool removePanel(const std::string &panelName);
    bool showPanel(const std::string &panelName, bool show);
    bool isPanelVisible(const std::string &panelName) const;
    std::vector<std::string> getActivePanels() const;

    // Layout Management
    bool saveLayout(const std::string &filename) const;
    bool loadLayout(const std::string &filename);
    void resetLayout();

    // Theme Management
    void setTheme(const editor::EditorTheme &theme);
    void setDefaultTheme();
    const editor::EditorTheme &getCurrentTheme() const
    {
        return currentTheme;
    }

    // Menu System
    void addMenuItem(const std::string &menu, const std::string &item, std::function<void()> callback);
    void addSubMenuItem(const std::string &menu, const std::string &item, const std::string &subItem,
                        std::function<void()> callback);

  private:
    void renderMenuBar();
    void applyTheme() const;

    std::unordered_map<std::string, std::unique_ptr<editor::EditorPanel>> panels;
    std::unordered_map<std::string, editor::PanelState> panelStates;
    std::unordered_map<std::string, std::vector<editor::MenuItem>> menuItems;
    editor::EditorTheme currentTheme;
};
} // namespace module