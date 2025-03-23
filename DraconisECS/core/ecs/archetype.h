#pragma once
#include <bitset>
#include <cassert>
#include <memory>
#include <tuple>
#include <vector>
#include "chunk.h"
#include "componentRegistry.h"
#include "ecsGlobals.h"
#include "entity.h"

namespace ecs
{
// Forward declarations
class Ecs;

// Archetype represents a unique combination of components
class Archetype
{
public:
	explicit Archetype( const std::bitset<MAX_COMPONENTS>& sig ) : signature( sig ), totalEntities( 0 ) {}

	const std::bitset<MAX_COMPONENTS>& getSignature() const
	{
		return signature;
	}

	void addEntity( const Entity& entity )
	{
		/*
		entityIds.push_back( entity.getId() );
		totalEntities++;
		*/
	}

	template <typename T>
	void addComponent( const Entity& entity, T&& component )
	{
		/*
		// Get or create a chunk with space for the new component
		Chunk& chunk = getOrCreateChunk();
		chunk.createComponent( entity.getEntityIndex(), std::forward<T>( component ) );
		*/
	}

	Chunk& getOrCreateChunk()
	{
		/*
		if( chunks.empty() || chunks.back()->isFull() )
		{
			chunks.push_back( std::make_unique<Chunk>() );
		}
		return *chunks.back();
		*/
	}

	void setLayout( ArchetypeLayout&& newLayout )
	{
		layout = std::move( newLayout );
	}
	const ArchetypeLayout& getLayout() const
	{
		return layout;
	}

private:
	ArchetypeLayout layout;
	std::bitset<MAX_COMPONENTS> signature;
	std::vector<std::unique_ptr<Chunk>> chunks;
	std::vector<size_t> entityIds;
	size_t totalEntities = 0;
};

}  // namespace ecs