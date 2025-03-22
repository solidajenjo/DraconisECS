#pragma once
#include <cstddef>

namespace ecs {
    // System-wide constants
    constexpr size_t CHUNK_SIZE = 16384;  // 16KB chunk size
    constexpr size_t MAX_COMPONENTS = 256;

    // Component type ID generation
    inline size_t typeIdSeq = 0;
    template<typename T> inline const size_t typeId = typeIdSeq++;

    // Component size tracking
    template<typename T> inline const size_t componentSize = sizeof(T);
} 