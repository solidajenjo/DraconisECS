#include "stylePanel.h"
#include "app.h"
#include <imgui.h>

namespace module::editor
{

StylePanel::StylePanel() = default;
StylePanel::~StylePanel() = default;

const char *StylePanel::getName() const
{
    return "Style Editor";
}

void StylePanel::render()
{
    if (!ImGui::Begin(getName(), nullptr))
    {
        ImGui::End();
        return;
    }

    auto &editor = app::appInstance.getModule<Editor>();
    auto theme = editor.getCurrentTheme();
    bool themeChanged = false;

    // Colors section
    if (ImGui::CollapsingHeader("Colors", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::ColorEdit4("Text", &theme.textColor.x))
            themeChanged = true;
        if (ImGui::ColorEdit4("Background", &theme.bgColor.x))
            themeChanged = true;

        ImGui::Separator();

        if (ImGui::ColorEdit4("Titlebar", &theme.titlebarColor.x))
            themeChanged = true;
        if (ImGui::ColorEdit4("Titlebar Active", &theme.titlebarActiveColor.x))
            themeChanged = true;

        ImGui::Separator();

        if (ImGui::ColorEdit4("Accent", &theme.accentColor.x))
            themeChanged = true;
        if (ImGui::ColorEdit4("Accent Active", &theme.accentActiveColor.x))
            themeChanged = true;

        ImGui::Separator();

        if (ImGui::ColorEdit4("Button", &theme.buttonColor.x))
            themeChanged = true;
        if (ImGui::ColorEdit4("Button Hover", &theme.buttonHoverColor.x))
            themeChanged = true;
        if (ImGui::ColorEdit4("Button Active", &theme.buttonActiveColor.x))
            themeChanged = true;
    }

    // Style section
    if (ImGui::CollapsingHeader("Style", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::SliderFloat("Corner Rounding", &theme.rounding, 0.0f, 12.0f, "%.1f"))
            themeChanged = true;
        if (ImGui::SliderFloat("Border Size", &theme.borderSize, 0.0f, 3.0f, "%.1f"))
            themeChanged = true;
    }

    ImGui::Separator();

    // Presets section
    if (ImGui::CollapsingHeader("Presets"))
    {
        if (ImGui::Button("Light Theme"))
        {
            editor.setDefaultTheme();
            themeChanged = false; // No need to apply changes since setDefaultTheme does it
        }

        ImGui::SameLine();

        if (ImGui::Button("Dark Theme"))
        {
            theme.textColor = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
            theme.bgColor = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
            theme.titlebarColor = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
            theme.titlebarActiveColor = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
            theme.accentColor = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
            theme.accentActiveColor = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            theme.buttonColor = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            theme.buttonHoverColor = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
            theme.buttonActiveColor = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
            themeChanged = true;
        }
    }

    if (themeChanged)
    {
        editor.setTheme(theme);
    }

    ImGui::End();
}

// Factory function implementation
std::unique_ptr<EditorPanel> createStylePanel()
{
    return std::make_unique<StylePanel>();
}

} // namespace module::editor