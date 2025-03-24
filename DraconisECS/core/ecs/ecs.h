#pragma once
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "archetype.h"
#include "chunk.h"
#include "componentRegistry.h"
#include "ecsGlobals.h"
#include "entity.h"

namespace ecs
{
// Ecs manages the entire ECS system
class Ecs
{
public:
	// Singleton instance
	static Ecs& getInstance()
	{
		static Ecs instance;
		return instance;
	}

	// Create entity with component initializers
	template <typename... Components>
	Entity createEntity( Components&&... components )
	{
		// register components
		( ComponentRegistry::getInstance().registerComponent<Components>(), ... );

		// create signature
		std::bitset<MAX_COMPONENTS> signature;
		( ( signature.set( ComponentRegistry::getInstance().getTypeId<Components>() ) ), ... );

		Entity entity( signature, 0, 0, entities.size() );
		entities.push_back( entity );
		auto it = archetypes.find( signature );
		Archetype* archetype;
		if( it == archetypes.end() )
		{
			// Create new archetype with the component types
			archetype = archetypes.emplace( signature, std::make_unique<Archetype>( signature ) ).first->second.get();
			ArchetypeLayout layout;
			( createArchetypeLayout<Components>( layout ), ... );
			std::sort( layout.typeOffsets.begin(), layout.typeOffsets.end(), []( const auto& a, const auto& b ) { return a.first < b.first; } );			
			size_t currentOffset = 0;
			for( auto& [typeId, typeOffset] : layout.typeOffsets )
			{
				typeOffset = currentOffset;
				currentOffset += ComponentRegistry::getInstance().getComponentSize(typeId);
			}
			archetype->setLayout( std::move( layout ) );
		}
		else
		{
			archetype = it->second.get();
		}

		// Add entity to archetype and initialize components
		archetype->addEntity( entity );
		( archetype->addComponent( entity, std::forward<Components>( components ) ), ... );
		std::cout << "Archetype layout: " << archetype->getLayout().totalSize << std::endl;
		//print layout
		for( const auto& [typeId, offset] : archetype->getLayout().typeOffsets )
		{
			std::cout << "Component: " << ComponentRegistry::getInstance().getComponentName(typeId)
					  << " Offset: " << offset 
					  << " Size: " << ComponentRegistry::getInstance().getComponentSize(typeId)
					  << " TypeId: " << typeId << std::endl;
		}

		return entity;
	}

	void destroyEntity( const Entity& entity );

	const std::vector<Entity>& getEntities() const
	{
		return entities;
	}

	/*

template <typename T>
T& getComponent( const Entity& entity )
{
	Archetype* archetype = getArchetypeBySignature( entity.getSignature() );
	assert( archetype != nullptr );
	return archetype->getComponent<T>( entity );
}
	*/

	template <typename T>
	void addComponent( Entity& entity, const T& initializer )
	{
		// Register component if not already registered
		( ComponentRegistry::getInstance().registerComponent<T>() );

		// Create new signature with added component
		std::bitset<MAX_COMPONENTS>& newSignature = entity.getSignature();
		newSignature.set( ComponentRegistry::getInstance().getTypeId<T>() );

		// Get or create new archetype
		auto it = archetypes.find( newSignature );
		Archetype* newArchetype;
		if( it == archetypes.end() )
		{
			newArchetype =
				archetypes.emplace( newSignature, std::make_unique<Archetype>( newSignature ) ).first->second.get();
		}
		else
		{
			newArchetype = it->second.get();
		}
		/*
		// Get or create chunk in new archetype
		Chunk& newChunk				= newArchetype->getOrCreateChunk();
		const size_t newEntityIndex = newChunk.getEntityCount();

		const size_t newChunkIndex =
			std::distance( newArchetype->getChunks().begin(),
						   std::find_if( newArchetype->getChunks().begin(),
										 newArchetype->getChunks().end(),
										 [&newChunk]( const auto& c ) { return c.get() == &newChunk; } ) );

		Archetype* oldArchetype = getArchetypeBySignature( entity.getSignature() );
		assert( oldArchetype );	 // entities without archetype not allowed
		*/
		/*
				const size_t sourceChunkComponentsSize = entity.getChunkIndex()
		   * ComponentRegistry::getInstance().getTotalComponentSize(
		   entity.getSignature() ); uint8_t* oldData =
		   &oldArchetype->chunks[entity.chunkIdx] .get()
										->data[entity.getChunkEntityIndex() *
		   sourceChunkComponentsSize];
		*/
		// oldArchetype->removeEntity(entity.getId())
		/*
		// Initialize new component
		newChunk.createComponent( newEntityIndex, initializer );

		// Update entity reference
		updateEntityReference( entity.getGlobalId(), newSignature, newChunkIndex, newEntityIndex );
		*/
		// Update chunk usage
		/*
		newChunk.incrementEntityCount();
		newChunk.setUsedSize(newChunk.getEntityCount() *
			ComponentRegistry::getInstance().getTotalComponentSize(newSignature));
			*/
	}

	template <typename T>
	void removeComponent( const Entity& entity )
	{
		/*
		// Prevent removing the default EntityIdComponent
		static_assert(!std::is_same_v<T, EntityIdComponent>, "Cannot remove the
		default EntityIdComponent");

		// Runtime check to prevent removing EntityIdComponent
		if (ComponentRegistry::getInstance().getTypeId<T>() ==
		ComponentRegistry::getInstance().getTypeId<EntityIdComponent>()) {
		assert(false && "Cannot remove the default EntityIdComponent at
		runtime"); return;
		}

		// Create new signature without the component
		std::bitset<MAX_COMPONENTS> newSignature = entity.getSignature();
		newSignature.reset(ComponentRegistry::getInstance().getTypeId<T>());

		// Get or create new archetype
		auto it = archetypes.find(newSignature);
		Archetype* newArchetype;
		if (it == archetypes.end()) {
			newArchetype = archetypes.emplace(newSignature,
		std::make_unique<Archetype>(newSignature)).first->second.get(); } else {
		newArchetype = it->second.get();
		}

		// Get or create chunk in new archetype
		Chunk& newChunk = newArchetype->getOrCreateChunk();
		size_t newChunkIndex = std::distance(newArchetype->getChunks().begin(),
			std::find_if(newArchetype->getChunks().begin(),
		newArchetype->getChunks().end(),
				[&newChunk](const auto& c) { return c.get() == &newChunk; }));
		size_t newEntityIndex = newChunk.getEntityCount();

		// Copy existing components except the one being removed
		Archetype* oldArchetype =
		getArchetypeBySignature(entity.getSignature()); if (oldArchetype) { for
		(size_t i = 0; i < MAX_COMPONENTS; ++i) { if
		(entity.getSignature().test(i) && i !=
		ComponentRegistry::getInstance().getTypeId<T>()) { size_t compSize =
		ComponentRegistry::getInstance().getComponentSize(i);
		newChunk.copyComponentDataFromChunk(
						*oldArchetype->getChunks()[entity.getChunkIndex()],
						entity.getEntityIndex(),
						newEntityIndex,
						compSize
					);
				}
			}
		}

		// Update entity reference
		updateEntityReference(entity.getId(), newSignature, newChunkIndex,
		newEntityIndex);

		// Update chunk usage
		newChunk.incrementEntityCount();
		newChunk.setUsedSize(newChunk.getEntityCount() *
			ComponentRegistry::getInstance().getTotalComponentSize(newSignature));
		*/
	}

private:
	template <typename Component>
	void createArchetypeLayout( ArchetypeLayout& layout )
	{
		std::cout << "Calculating archetype layout for component: " << ComponentRegistry::getInstance().getTypeName<Component>() << std::endl;		
		layout.typeOffsets.emplace_back( ComponentRegistry::getInstance().getTypeId<Component>(), 0 );
		layout.totalSize += sizeof( Component );
	}

	std::unordered_map<std::bitset<MAX_COMPONENTS>, std::unique_ptr<Archetype>> archetypes;
	std::vector<Entity> entities;

	// Make Archetype and Entity friends to allow access to private methods
	

	/*
	// Helper function to initialize a single component with its initializer
	template <typename T>
	void initializeComponent( Archetype* archetype, size_t entityIndex, const T& initializer )
	{
		// Get the chunk that this entity belongs to
		Chunk& chunk = archetype->getOrCreateChunk();
		chunk.createComponent( entityIndex, initializer );
	}

	// Helper function to get archetype by signature
	Archetype* getArchetypeBySignature( const std::bitset<MAX_COMPONENTS>& signature );

	// Helper function to get archetype by signature (const version)
	const Archetype* getArchetypeBySignature( const std::bitset<MAX_COMPONENTS>& signature ) const;
	*/
};
}  // namespace ecs