#include "renderEP.h"
#include "imgui.h"

void editorPanel::RenderEP::render() const
{
	ImGui::Begin("Render");
	ImColor color = ImColor(114, 144, 154);
	ImGui::TextColored(color, "Hello, world!");
	ImGui::End();
}
