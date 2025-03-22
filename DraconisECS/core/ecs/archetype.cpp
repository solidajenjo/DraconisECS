#include "archetype.h"
#include "ecs.h"
#include <cassert>

namespace ecs {

Archetype::Archetype(const std::bitset<MAX_COMPONENTS>& sig) : signature(sig) {}

Archetype::~Archetype() = default;

const std::bitset<MAX_COMPONENTS>& Archetype::getSignature() const { 
    return signature; 
}

size_t Archetype::getEntityCount() const { 
    return entityCount; 
}

const std::vector<size_t>& Archetype::getEntityIds() const { 
    return entityIds; 
}

const std::vector<std::unique_ptr<Chunk>>& Archetype::getChunks() const { 
    return chunks; 
}

std::vector<std::unique_ptr<Chunk>>& Archetype::getMutableChunks() { 
    return chunks; 
}

void Archetype::addEntity(size_t entityId) {
    entityIds.push_back(entityId);
    entityCount++;
}

template<typename T>
T& Archetype::getComponent(size_t index) {
    assert(signature.test(typeId<T>));
    assert(!chunks.empty());
    assert(index < entityCount);
    
    size_t entitiesPerChunk = chunks[0]->getMaxEntities();
    size_t chunkIndex = index / entitiesPerChunk;
    size_t entityIndex = index % entitiesPerChunk;
    
    return chunks[chunkIndex]->getComponent<T>(entityIndex);
}

template<typename T>
const T& Archetype::getComponent(size_t index) const {
    assert(signature.test(typeId<T>));
    assert(!chunks.empty());
    assert(index < entityCount);
    
    size_t entitiesPerChunk = chunks[0]->getMaxEntities();
    size_t chunkIndex = index / entitiesPerChunk;
    size_t entityIndex = index % entitiesPerChunk;
    
    return chunks[chunkIndex]->getComponent<T>(entityIndex);
}

void Archetype::removeEntity(size_t index) {
    if (index < entityCount) {
        size_t entitiesPerChunk = chunks[0]->getMaxEntities();
        size_t chunkIndex = index / entitiesPerChunk;
        size_t entityIndex = index % entitiesPerChunk;
        Chunk& chunk = *chunks[chunkIndex];

        // If not the last entity, move the last entity's data to this position
        if (index < entityCount - 1) {
            // Move entity ID
            std::swap(entityIds[index], entityIds[entityCount - 1]);
            
            // Move component data for each component type
            for (size_t comp = 0; comp < MAX_COMPONENTS; ++comp) {
                if (signature.test(comp)) {
                    size_t compSize = ComponentRegistry::getInstance().getComponentSize(comp);
                    chunk.copyComponentData(entityCount - 1, entityIndex, compSize);
                }
            }
        }

        entityIds.pop_back();
        entityCount--;
    }
}

Chunk& Archetype::getOrCreateChunk() {
    // Check if last chunk has space
    if (!chunks.empty() && chunks.back()->hasSpace()) {
        return *chunks.back();
    }

    // Create new chunk if needed
    chunks.push_back(std::make_unique<Chunk>(*this));
    return *chunks.back();
}

} // namespace ecs 