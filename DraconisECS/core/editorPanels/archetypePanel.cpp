#include "archetypePanel.h"
#include <imgui.h>
#include "tests/testECS.h"

namespace module::editor
{
const char* ArchetypePanel::getName() const
{
	return "Archetype Viewer";
}

void ArchetypePanel::render()
{
	if( !isOpen )
		return;

	ImGui::Begin( getName(), &isOpen );

	// Get reference to ECS system
	auto& ecs = ecs::Ecs::getInstance();

	// Add helper function to get archetype name
	auto getArchetypeName = []( const std::bitset<::ecs::MAX_COMPONENTS>& signature ) -> std::string {
		std::vector<std::string> components;
		for( size_t i = 0; i < ::ecs::MAX_COMPONENTS; ++i )
		{
			if( signature.test( i ) )
			{
				components.push_back( ::ecs::ComponentRegistry::getInstance().getComponentName( i ) );
			}
		}
		std::string name;
		for( size_t i = 0; i < components.size(); ++i )
		{
			name += components[i];
			if( i < components.size() - 1 )
			{
				name += ", ";
			}
		}
		return name;
	};

	/*
		// Display archetypes
		for( const auto& [signature, archetype] : ecs.getArchetypes() )
		{
			if( ImGui::CollapsingHeader( getArchetypeName( signature ).c_str() )
	   )
			{
				ImGui::Indent();
				ImGui::Text( "Entity Count: %zu", archetype->getEntityCount() );
				ImGui::Text( "Chunk Count: %zu", archetype->getChunks().size()
	   ); ImGui::Unindent();
			}
		}
	*/

	ImGui::End();
}
}  // namespace module::editor