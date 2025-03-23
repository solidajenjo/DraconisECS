#pragma once
#include <bitset>
#include "componentRegistry.h"
#include "ecsGlobals.h"

namespace ecs
{
// Default component that every entity must have to ensure it has an archetype
struct EntityComponent
{
};

class Entity
{
public:
	Entity( const std::bitset<MAX_COMPONENTS>& sig, size_t chunkIdx, size_t entityChunkIdx, size_t globalIdx )
		: signature( sig ), chunkIdx( chunkIdx ), entityChunkIdx( entityChunkIdx ), globalIdx( globalIdx )
	{
		signature.set( ComponentRegistry::getInstance().getTypeId<EntityComponent>() );
	}

	void updateReferences( const std::bitset<MAX_COMPONENTS>& newSig, size_t newChunkIdx, size_t newEntityChunkIdx )
	{
		signature	   = newSig;
		chunkIdx	   = newChunkIdx;
		entityChunkIdx = newEntityChunkIdx;
	}

	const std::bitset<MAX_COMPONENTS>& getSignature() const
	{
		return signature;
	}
	size_t getChunkIndex() const
	{
		return chunkIdx;
	}
	size_t getEntityChunkIdx() const
	{
		return entityChunkIdx;
	}
	size_t getGlobalId() const
	{
		return globalIdx;
	}

private:
	friend class Ecs;
	// friend class Archetype;

	void setSignature( const std::bitset<MAX_COMPONENTS>& sig )
	{
		signature = sig;
	}

	void setChunkIndex( size_t idx )
	{
		chunkIdx = idx;
	}
	void setEntityChunkIdx( size_t idx )
	{
		entityChunkIdx = idx;
	}

	std::bitset<MAX_COMPONENTS>& getSignature()
	{
		return signature;
	}

	std::bitset<MAX_COMPONENTS> signature;
	size_t chunkIdx;
	size_t entityChunkIdx;
	size_t globalIdx;
};
}  // namespace ecs