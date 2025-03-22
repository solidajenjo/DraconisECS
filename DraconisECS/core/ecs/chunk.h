#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <bitset>
#include "componentRegistry.h"

namespace ecs {

    // Forward declarations
    class Archetype;

    // Chunk represents a fixed-size block of memory for component data
    class Chunk {
    public:
        Chunk(Archetype& arch);
        ~Chunk();

        bool hasSpace() const;
        size_t getMaxEntities() const;
        Archetype& getArchetype() const;
        size_t getUsedSize() const;
        size_t getEntityCount() const;

        template<typename T>
        void createComponent(size_t index, const T& initializer) {
            size_t offset = index * sizeof(T);
            new (data.data() + offset) T(initializer);
        }

    private:
        Archetype& archetype;
        std::vector<uint8_t> data;  // Fixed-size chunk data
        size_t usedSize = 0;
        size_t entityCount = 0;

        // Make Archetype and Ecs friends to allow them to access private methods
        friend class Archetype;
        friend class Ecs;

        template<typename T>
        T& getComponent(size_t index);
        template<typename T>
        const T& getComponent(size_t index) const;

        void copyComponentData(size_t sourceIndex, size_t targetIndex, size_t componentSize);
        void copyComponentDataFromChunk(const Chunk& sourceChunk, size_t sourceIndex, size_t targetIndex, size_t componentSize);
        const std::vector<uint8_t>& getData() const;
        void setUsedSize(size_t size);
        void setEntityCount(size_t count);
        void incrementEntityCount();
        void decrementEntityCount();
    };
} 