#pragma once
#include <cstddef>
#include <typeindex>
#include <vector>
#include <utility>


namespace ecs
{
// System-wide constants
constexpr size_t CHUNK_SIZE		= 16384;  // 16KB chunk size
constexpr size_t MAX_COMPONENTS = 256;

// Component size tracking
template <typename T>
inline const size_t componentSize = sizeof( T );

struct ArchetypeLayout
{
	std::vector<std::pair<size_t, size_t>> typeOffsets;  // typeId, offset
	size_t totalSize = 0;
};

}  // namespace ecs