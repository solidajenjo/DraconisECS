#include "configPanel.h"
#include <SDL.h>
#include <imgui.h>
#include <filesystem>
#include <iostream>

namespace module::editor
{
void ConfigPanel::render()
{
	if( ImGui::Begin( "Configuration" ) )
	{
		auto& config   = core::Config::get();
		bool needsSave = false;

		if( ImGui::CollapsingHeader( "Window", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			renderWindowConfig();
			if( isDirty )
				needsSave = true;
		}

		if( ImGui::CollapsingHeader( "Graphics", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			renderGraphicsConfig();
			if( isDirty )
				needsSave = true;
		}

		if( needsSave )
		{
			ImGui::Separator();
			if( ImGui::Button( "Save Changes" ) )
			{
				config.save();
				isDirty = false;
			}
		}
	}
	ImGui::End();
}

void ConfigPanel::renderWindowConfig()
{
	auto& config	   = core::Config::get();
	auto& windowConfig = config.getWindowConfig();

	int width		= windowConfig.width;
	int height		= windowConfig.height;
	bool fullscreen = windowConfig.fullscreen;

	if( ImGui::InputInt( "Width", &width ) )
	{
		windowConfig.width = width;
		isDirty			   = true;
	}

	if( ImGui::InputInt( "Height", &height ) )
	{
		windowConfig.height = height;
		isDirty				= true;
	}

	if( ImGui::Checkbox( "Fullscreen", &fullscreen ) )
	{
		windowConfig.fullscreen = fullscreen;
		isDirty					= true;
	}
}

void ConfigPanel::renderGraphicsConfig()
{
	auto& config = core::Config::get();

	// Graphics settings
	if( ImGui::CollapsingHeader( "Graphics", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::Indent( 10 );
		ImGui::Unindent( 10 );

		// OpenGL version
		int glMajor = config.getGraphicsConfig().glMajorVersion;
		int glMinor = config.getGraphicsConfig().glMinorVersion;
		if( ImGui::DragInt2( "OpenGL Version", &glMajor, 1, 1, 4, "%d" ) )
		{
			config.getGraphicsConfig().glMajorVersion = glMajor;
			config.getGraphicsConfig().glMinorVersion = glMinor;
			config.save();
		}

		// MSAA samples
		int msaaSamples = config.getGraphicsConfig().msaaSamples;
		if( ImGui::DragInt( "MSAA Samples", &msaaSamples, 1, 0, 16, "%d" ) )
		{
			config.getGraphicsConfig().msaaSamples = msaaSamples;
			config.save();
		}

		// VSync
		bool vsync = config.getWindowConfig().vsync;
		if( ImGui::Checkbox( "VSync", &vsync ) )
		{
			config.getWindowConfig().vsync = vsync;
			config.save();
		}
	}
}
}  // namespace module::editor