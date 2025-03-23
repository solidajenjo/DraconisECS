#pragma once
#include <bitset>
#include <cstdint>
#include <memory>
#include <vector>
#include "componentRegistry.h"

namespace ecs
{
// class Archetype;

// Chunk represents a fixed-size block of memory for component data
class Chunk
{
public:
	// Chunk( Archetype& arch );
	~Chunk();

	size_t getUsedSize() const;
	size_t getMaxEntities() const;
	size_t getEntityCount() const;

private:
	// Make Archetype and Ecs friends to allow them to access private methods
	// friend class Archetype;
	friend class Ecs;

	bool hasSpace() const;
	// Archetype& getArchetype() const;

	template <typename T>
	void createComponent( size_t index, const T& initializer )
	{
		size_t offset = index * sizeof( T );
		new( data.data() + offset ) T( initializer );
	}

	template <typename T>
	T& getComponent( size_t index )
	{
		size_t offset = index * sizeof( T );
		return *reinterpret_cast<T*>( data.data() + offset );
	}

	template <typename T>
	const T& getComponent( size_t index ) const
	{
		size_t offset = index * sizeof( T );
		return *reinterpret_cast<const T*>( data.data() + offset );
	}

	// Archetype& archetype;
	std::vector<uint8_t> data;	// Fixed-size chunk data
	size_t usedSize	   = 0;
	size_t entityCount = 0;

	std::vector<uint8_t>& getData();
	void setUsedSize( size_t size );
	void setEntityCount( size_t count );
};
}  // namespace ecs