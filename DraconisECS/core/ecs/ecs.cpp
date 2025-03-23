#include "ecs.h"
#include <algorithm>
#include <cassert>

namespace ecs
{

void Ecs::destroyEntity( const Entity& entity )
{
	// Find archetype containing this entity
	for( auto& [signature, archetype] : archetypes )
	{
		/*
		if (archetype->hasEntity(entity))
		{
			archetype->removeEntity(entity);
			break;
		}
		*/
	}
}

}  // namespace ecs