#include "stylePanel.h"
#include <imgui.h>
#include "app.h"

namespace module::editor
{
StylePanel::StylePanel()  = default;
StylePanel::~StylePanel() = default;

const char* StylePanel::getName() const
{
	return "Style Editor";
}

void StylePanel::render()
{
	if( !ImGui::Begin( getName(), nullptr ) )
	{
		ImGui::End();
		return;
	}

	auto& editor	  = app::appInstance.getModule<Editor>();
	auto theme		  = editor.getCurrentTheme();
	bool themeChanged = false;

	// Colors section
	if( ImGui::CollapsingHeader( "Colors", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::Indent();
		if( ImGui::ColorEdit4( "Text Color", (float*)&theme.textColor ) )
			themeChanged = true;
		if( ImGui::ColorEdit4( "Background Color", (float*)&theme.bgColor ) )
			themeChanged = true;
		if( ImGui::ColorEdit4( "Titlebar Color", (float*)&theme.titlebarColor ) )
			themeChanged = true;
		if( ImGui::ColorEdit4( "Titlebar Active Color", (float*)&theme.titlebarActiveColor ) )
			themeChanged = true;
		if( ImGui::ColorEdit4( "Accent Color", (float*)&theme.accentColor ) )
			themeChanged = true;
		if( ImGui::ColorEdit4( "Accent Active Color", (float*)&theme.accentActiveColor ) )
			themeChanged = true;
		if( ImGui::ColorEdit4( "Button Color", (float*)&theme.buttonColor ) )
			themeChanged = true;
		if( ImGui::ColorEdit4( "Button Hover Color", (float*)&theme.buttonHoverColor ) )
			themeChanged = true;
		if( ImGui::ColorEdit4( "Button Active Color", (float*)&theme.buttonActiveColor ) )
			themeChanged = true;
		ImGui::Unindent();
	}

	// Layout section
	if( ImGui::CollapsingHeader( "Layout", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::Indent();
		if( ImGui::SliderFloat( "Rounding", &theme.rounding, 0.0f, 10.0f ) )
			themeChanged = true;
		if( ImGui::SliderFloat( "Border Size", &theme.borderSize, 0.0f, 2.0f ) )
			themeChanged = true;
		ImGui::Unindent();
	}

	// Presets section
	if( ImGui::CollapsingHeader( "Presets" ) )
	{
		if( ImGui::Button( "Light Theme" ) )
		{
			editor.setDefaultTheme();
			themeChanged = false;  // No need to apply changes since
								   // setDefaultTheme does it
		}

		ImGui::SameLine();

		if( ImGui::Button( "Dark Theme" ) )
		{
			theme.textColor			  = ImVec4( 0.90f, 0.90f, 0.90f, 1.00f );
			theme.bgColor			  = ImVec4( 0.15f, 0.15f, 0.15f, 1.00f );
			theme.titlebarColor		  = ImVec4( 0.10f, 0.10f, 0.10f, 1.00f );
			theme.titlebarActiveColor = ImVec4( 0.15f, 0.15f, 0.15f, 1.00f );
			theme.accentColor		  = ImVec4( 0.26f, 0.59f, 0.98f, 0.67f );
			theme.accentActiveColor	  = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
			theme.buttonColor		  = ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );
			theme.buttonHoverColor	  = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
			theme.buttonActiveColor	  = ImVec4( 0.30f, 0.30f, 0.30f, 1.00f );
			themeChanged			  = true;
		}
	}

	if( themeChanged )
	{
		editor.setTheme( theme );
	}

	ImGui::End();
}

// Factory function implementation
std::unique_ptr<EditorPanel> createStylePanel()
{
	return std::make_unique<StylePanel>();
}
}  // namespace module::editor