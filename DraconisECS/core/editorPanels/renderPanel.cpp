#include "renderPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>

namespace module::editor
{
RenderPanel::RenderPanel( module::Render* renderModule ) : m_RenderModule( renderModule ), m_LastSize( 0, 0 )
{
	// No need to create a framebuffer here as we'll use the one from render
	// module
	std::cout << "Created RenderPanel using render module's framebuffer" << std::endl;
}

void RenderPanel::render()
{
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );

	if( ImGui::Begin( getName(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ) )
	{
		// Get the available content region size
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		// Check if the viewport size has changed
		if( viewportSize.x != m_LastSize.x || viewportSize.y != m_LastSize.y )
		{
			// Only resize if both dimensions are greater than 0
			if( viewportSize.x > 0 && viewportSize.y > 0 )
			{
				std::cout << "Resizing render module's framebuffer to: " << viewportSize.x << "x" << viewportSize.y
						  << std::endl;
				m_RenderModule->getDefaultFramebuffer()->resize( static_cast<int>( viewportSize.x ),
																 static_cast<int>( viewportSize.y ) );
				m_LastSize = viewportSize;
			}
		}

		// Get the framebuffer texture ID
		/* TODO: FIX THIS
		//ImTextureID textureId = (ImTextureID)(uintptr_t)m_RenderModule->getDefaultFramebuffer()->getColorTexture();

		// Render the framebuffer texture
		ImGui::Image( textureId, viewportSize, ImVec2( 0, 1 ), ImVec2( 1, 0 ) );

		// Add debug overlay
		ImGui::SetCursorPos( ImVec2( 10, 10 ) );
		ImGui::Text( "Framebuffer Size: %.0fx%.0f", viewportSize.x, viewportSize.y );
		ImGui::Text( "Texture ID: %u", (GLuint)(uintptr_t)textureId );
		ImGui::Text( "Framebuffer ID: %u", m_RenderModule->getDefaultFramebuffer()->getFramebufferID() );
		*/
		// Check OpenGL errors
		GLenum err;
		while( ( err = glGetError() ) != GL_NO_ERROR )
		{
			std::cout << "OpenGL error in RenderPanel: 0x" << std::hex << err << std::dec << std::endl;
		}
	}
	ImGui::End();

	ImGui::PopStyleVar();
}
}  // namespace module::editor
