#include "chunk.h"
#include "ecs.h"
#include <cstring>

namespace ecs {

Chunk::Chunk(Archetype& arch) : archetype(arch) {
    data.resize(CHUNK_SIZE);
}

Chunk::~Chunk() = default;

bool Chunk::hasSpace() const {
    return entityCount < getMaxEntities();
}

size_t Chunk::getMaxEntities() const {
    return CHUNK_SIZE / ComponentRegistry::getInstance().getTotalComponentSize(archetype.getSignature());
}

Archetype& Chunk::getArchetype() const { 
    return archetype; 
}

size_t Chunk::getUsedSize() const { 
    return usedSize; 
}

size_t Chunk::getEntityCount() const { 
    return entityCount; 
}

template<typename T>
T& Chunk::getComponent(size_t index) {
    return *reinterpret_cast<T*>(data.data() + index * sizeof(T));
}

template<typename T>
const T& Chunk::getComponent(size_t index) const {
    return *reinterpret_cast<const T*>(data.data() + index * sizeof(T));
}

void Chunk::copyComponentData(size_t sourceIndex, size_t targetIndex, size_t componentSize) {
    std::memcpy(
        data.data() + targetIndex * componentSize,
        data.data() + sourceIndex * componentSize,
        componentSize
    );
}

void Chunk::copyComponentDataFromChunk(const Chunk& sourceChunk, size_t sourceIndex, size_t targetIndex, size_t componentSize) {
    std::memcpy(
        data.data() + targetIndex * componentSize,
        sourceChunk.getData().data() + sourceIndex * componentSize,
        componentSize
    );
}

const std::vector<uint8_t>& Chunk::getData() const { 
    return data; 
}

void Chunk::setUsedSize(size_t size) { 
    usedSize = size; 
}

void Chunk::setEntityCount(size_t count) { 
    entityCount = count; 
}

void Chunk::incrementEntityCount() { 
    entityCount++; 
}

void Chunk::decrementEntityCount() { 
    entityCount--; 
}

} // namespace ecs 