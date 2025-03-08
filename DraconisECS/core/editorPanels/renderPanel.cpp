#include "renderPanel.h"
#include <imgui.h>

namespace module::editor
{
void RenderPanel::render()
{
    ImGui::Begin("Render");
    ImColor color = ImColor(114, 144, 154);
    ImGui::TextColored(color, "Hello, world!");
    ImGui::End();
}
} // namespace module::editor
