#include "chunk.h"
#include <cstring>
#include "ecs.h"

namespace ecs
{
/*
Chunk::Chunk( Archetype& arch ) : archetype( arch )
{
data.resize( CHUNK_SIZE );
}
*/

Chunk::~Chunk() = default;

bool Chunk::hasSpace() const
{
	return entityCount < getMaxEntities();
}
/*
size_t Chunk::getMaxEntities() const
{
	assert( archetype.getSignature().any() );
	return CHUNK_SIZE / ComponentRegistry::getInstance().getTotalComponentSize( archetype.getSignature() );
}

Archetype& Chunk::getArchetype() const
{
	return archetype;
}
*/

size_t Chunk::getUsedSize() const
{
	return usedSize;
}

size_t Chunk::getEntityCount() const
{
	return entityCount;
}

std::vector<uint8_t>& Chunk::getData()
{
	return data;
}

void Chunk::setUsedSize( size_t size )
{
	usedSize = size;
}

void Chunk::setEntityCount( size_t count )
{
	entityCount = count;
}

size_t Chunk::getMaxEntities() const
{
	/*
	assert( archetype.getSignature().any() );
	return CHUNK_SIZE / ComponentRegistry::getInstance().getTotalComponentSize( archetype.getSignature() );
	*/
	return 0;
}
}  // namespace ecs