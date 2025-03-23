#pragma once
#include <bitset>
#include <typeindex>
#include <unordered_map>
#include "ecsGlobals.h"

namespace ecs
{
// Component registry for runtime size lookup
class ComponentRegistry
{
public:
	static ComponentRegistry& getInstance()
	{
		static ComponentRegistry instance;
		return instance;
	}

	template <typename T>
	size_t getTypeId()
	{
		static size_t id = nextId++;
		return id;
	}

	template <typename T>
	void registerComponent()
	{
		size_t id = getTypeId<T>();
		sizes[id] = sizeof( T );
		names[id] = typeid( T ).name();
	}

	template <typename T>
	const char* getTypeName()
	{
		size_t id = getTypeId<T>();
		return getComponentName( id );
	}

	size_t getComponentSize( size_t typeId ) const
	{
		auto it = sizes.find( typeId );
		return it != sizes.end() ? it->second : 0;
	}

	const char* getComponentName( size_t typeId ) const
	{
		auto it = names.find( typeId );
		return it != names.end() ? it->second.c_str() : "Unknown";
	}

	size_t getTotalComponentSize( const std::bitset<MAX_COMPONENTS>& signature ) const
	{
		size_t totalSize = 0;
		for( size_t i = 0; i < MAX_COMPONENTS; ++i )
		{
			if( signature.test( i ) )
			{
				totalSize += getComponentSize( i );
			}
		}
		return totalSize;
	}

private:
	ComponentRegistry()										 = default;
	~ComponentRegistry()									 = default;
	ComponentRegistry( const ComponentRegistry& )			 = delete;
	ComponentRegistry& operator=( const ComponentRegistry& ) = delete;

	std::unordered_map<size_t, size_t> sizes;
	std::unordered_map<size_t, std::string> names;
	size_t nextId = 0;
};
}  // namespace ecs