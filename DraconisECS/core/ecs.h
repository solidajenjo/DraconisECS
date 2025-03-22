#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cstdint>
#include <tuple>

#define CHUNK_SIZE 16384
#define MAX_COMPONENTS 256
#define INITIAL_ENTITY_CAPACITY 64

namespace ecs
{
    // Component type ID generation
    inline size_t typeIdSeq = 0;
    template<typename T> inline const size_t typeId = typeIdSeq++;

    // Forward declarations
    class Archetype;
    class Chunk;

    // Entity represents a unique ID with its archetype
    struct Entity {
        size_t id;
        Archetype* archetype;
        size_t chunkIndex;
        size_t entityIndex;

        Entity(size_t entityId, Archetype* arch, size_t chunkIdx, size_t entityIdx)
            : id(entityId), archetype(arch), chunkIndex(chunkIdx), entityIndex(entityIdx) {}
    };

    // Chunk represents a fixed-size block of entities with the same archetype
    class Chunk {
    public:
        Archetype* archetype;
        size_t usedSize = 0;

        bool hasSpace() const {
            return usedSize < CHUNK_SIZE;
        }

        template<typename T>
        T* getComponent(size_t index) {
            return archetype->getComponent<T>(index);
        }
    };

    // Archetype represents a unique combination of components
    class Archetype {
    public:
        std::bitset<MAX_COMPONENTS> signature;  // Component signature
        std::vector<std::vector<uint8_t>> componentData;  // Raw component data
        std::vector<size_t> entityIds;  // Entity IDs in this archetype
        std::vector<std::unique_ptr<Chunk>> chunks;  // Owned chunks
        size_t entityCount = 0;

        Archetype(const std::bitset<MAX_COMPONENTS>& sig) : signature(sig) {
            // Pre-allocate component data vectors based on signature
            componentData.resize(MAX_COMPONENTS);
            for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                if (signature.test(i)) {
                    componentData[i].reserve(INITIAL_ENTITY_CAPACITY);
                }
            }
            entityIds.reserve(INITIAL_ENTITY_CAPACITY);
        }

        template<typename T>
        T* getComponent(size_t index) {
            return reinterpret_cast<T*>(componentData[typeId<T>].data() + index * sizeof(T));
        }

        void addEntity(size_t entityId) {
            // Resize component data if needed
            if (entityCount >= entityIds.capacity()) {
                size_t newCapacity = entityIds.capacity() * 2;
                entityIds.reserve(newCapacity);
                for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                    if (signature.test(i)) {
                        componentData[i].reserve(newCapacity);
                    }
                }
            }
            entityIds.push_back(entityId);
            entityCount++;
        }

        void removeEntity(size_t index) {
            // Move last entity to this position
            if (index < entityCount - 1) {
                entityIds[index] = entityIds[entityCount - 1];
                // Move component data
                for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                    if (signature.test(i)) {
                        std::memcpy(componentData[i].data() + index * sizeof(uint8_t),
                                  componentData[i].data() + (entityCount - 1) * sizeof(uint8_t),
                                  sizeof(uint8_t));
                    }
                }
            }
            entityCount--;
        }

        Chunk* getOrCreateChunk() {
            // Find chunk with space
            for (auto& chunk : chunks) {
                if (chunk->hasSpace()) {
                    return chunk.get();
                }
            }
            // Create new chunk if needed
            chunks.push_back(std::make_unique<Chunk>());
            chunks.back()->archetype = this;
            return chunks.back().get();
        }
    };

    // Ecs manages the entire ECS system
    class Ecs {
    public:
        std::unordered_map<std::bitset<MAX_COMPONENTS>, std::unique_ptr<Archetype>> archetypes;
        std::vector<Entity> entities;
        size_t nextEntityId = 0;

        // Helper function to initialize a single component with its initializer
        template<typename T>
        void initializeComponent(Archetype* archetype, size_t entityIndex, const T& initializer) {
            new (archetype->getComponent<T>(entityIndex)) T(initializer);
        }

        // Create entity with component initializers
        template<typename... Components>
        Entity createEntity(const Components&... initializers) {
            // Create signature for this component combination
            std::bitset<MAX_COMPONENTS> signature;
            ((signature.set(typeId<Components>), ...));

            // Find or create archetype
            auto it = archetypes.find(signature);
            Archetype* archetype;
            if (it == archetypes.end()) {
                archetype = archetypes.emplace(signature, std::make_unique<Archetype>(signature)).first->second.get();
            } else {
                archetype = it->second.get();
            }

            // Get or create chunk
            Chunk* chunk = archetype->getOrCreateChunk();
            size_t chunkIndex = std::distance(archetype->chunks.begin(),
                std::find_if(archetype->chunks.begin(), archetype->chunks.end(),
                    [chunk](const auto& c) { return c.get() == chunk; }));

            // Create entity
            size_t entityIndex = chunk->usedSize++;
            Entity entity(nextEntityId++, archetype, chunkIndex, entityIndex);
            entities.push_back(entity);

            // Initialize components with initializers
            archetype->addEntity(entity.id);
            (initializeComponent(archetype, entityIndex, initializers), ...);

            return entity;
        }

        template<typename T>
        T* getComponent(const Entity& entity) {
            return entity.archetype->getComponent<T>(entity.entityIndex);
        }

        void destroyEntity(const Entity& entity) {
            // Remove from archetype
            entity.archetype->removeEntity(entity.entityIndex);
            
            // Remove from entities list
            auto it = std::find_if(entities.begin(), entities.end(),
                [&](const Entity& e) { return e.id == entity.id; });
            if (it != entities.end()) {
                entities.erase(it);
            }
        }

        // Add component with initializer
        template<typename T>
        T& addComponent(const Entity& entity, const T& initializer) {
            // Create new signature with added component
            std::bitset<MAX_COMPONENTS> newSignature = entity.archetype->signature;
            newSignature.set(typeId<T>);

            // Find or create new archetype
            auto it = archetypes.find(newSignature);
            Archetype* newArchetype;
            if (it == archetypes.end()) {
                newArchetype = archetypes.emplace(newSignature, std::make_unique<Archetype>(newSignature)).first->second.get();
            } else {
                newArchetype = it->second.get();
            }

            // Get or create chunk in new archetype
            Chunk* newChunk = newArchetype->getOrCreateChunk();
            size_t newChunkIndex = std::distance(newArchetype->chunks.begin(),
                std::find_if(newArchetype->chunks.begin(), newArchetype->chunks.end(),
                    [newChunk](const auto& c) { return c.get() == newChunk; }));

            // Copy all components to new archetype
            size_t newEntityIndex = newChunk->usedSize++;
            newArchetype->addEntity(entity.id);

            // Copy existing components
            for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                if (entity.archetype->signature.test(i)) {
                    std::memcpy(newArchetype->componentData[i].data() + newEntityIndex * sizeof(uint8_t),
                              entity.archetype->componentData[i].data() + entity.entityIndex * sizeof(uint8_t),
                              sizeof(uint8_t));
                }
            }

            // Initialize new component with initializer and get reference
            T& newComponent = *new (newArchetype->getComponent<T>(newEntityIndex)) T(initializer);

            // Remove from old archetype
            entity.archetype->removeEntity(entity.entityIndex);

            // Update entity reference
            auto& entityRef = *std::find_if(entities.begin(), entities.end(),
                [&](Entity& e) { return e.id == entity.id; });
            entityRef.archetype = newArchetype;
            entityRef.chunkIndex = newChunkIndex;
            entityRef.entityIndex = newEntityIndex;

            return newComponent;
        }

        template<typename T>
        void removeComponent(const Entity& entity) {
            // Create new signature without the component
            std::bitset<MAX_COMPONENTS> newSignature = entity.archetype->signature;
            newSignature.reset(typeId<T>);

            // Find or create new archetype
            auto it = archetypes.find(newSignature);
            Archetype* newArchetype;
            if (it == archetypes.end()) {
                newArchetype = archetypes.emplace(newSignature, std::make_unique<Archetype>(newSignature)).first->second.get();
            } else {
                newArchetype = it->second.get();
            }

            // Get or create chunk in new archetype
            Chunk* newChunk = newArchetype->getOrCreateChunk();
            size_t newChunkIndex = std::distance(newArchetype->chunks.begin(),
                std::find_if(newArchetype->chunks.begin(), newArchetype->chunks.end(),
                    [newChunk](const auto& c) { return c.get() == newChunk; }));

            // Copy all components except the one being removed
            size_t newEntityIndex = newChunk->usedSize++;
            newArchetype->addEntity(entity.id);

            // Copy existing components
            for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                if (entity.archetype->signature.test(i) && i != typeId<T>) {
                    std::memcpy(newArchetype->componentData[i].data() + newEntityIndex * sizeof(uint8_t),
                              entity.archetype->componentData[i].data() + entity.entityIndex * sizeof(uint8_t),
                              sizeof(uint8_t));
                }
            }

            // Remove from old archetype
            entity.archetype->removeEntity(entity.entityIndex);

            // Update entity reference
            auto& entityRef = *std::find_if(entities.begin(), entities.end(),
                [&](Entity& e) { return e.id == entity.id; });
            entityRef.archetype = newArchetype;
            entityRef.chunkIndex = newChunkIndex;
            entityRef.entityIndex = newEntityIndex;
        }
    };

} // namespace ecs