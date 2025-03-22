#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cstdint>
#include <tuple>

namespace ecs
{
    constexpr size_t CHUNK_SIZE = 16384;  // 16KB chunk size
    constexpr size_t MAX_COMPONENTS = 256;
    constexpr size_t INITIAL_ENTITY_CAPACITY = 64;

    // Component type ID generation
    inline size_t typeIdSeq = 0;
    template<typename T> inline const size_t typeId = typeIdSeq++;

    // Component size tracking
    template<typename T> inline const size_t componentSize = sizeof(T);

    // Forward declarations
    class Archetype;
    class Chunk;
    class Ecs;

    // Entity represents a unique ID with its archetype
    struct Entity {
        size_t id;
        Archetype* archetype;
        size_t chunkIndex;
        size_t entityIndex;

        Entity(size_t entityId, Archetype* arch, size_t chunkIdx, size_t entityIdx)
            : id(entityId), archetype(arch), chunkIndex(chunkIdx), entityIndex(entityIdx) {}
    };

    // Component registry for runtime size lookup
    class ComponentRegistry {
    public:
        static ComponentRegistry& getInstance() {
            static ComponentRegistry instance;
            return instance;
        }

        template<typename T>
        void registerComponent() {
            size_t id = typeId<T>;
            sizes[id] = sizeof(T);
            names[id] = typeid(T).name();
        }

        size_t getComponentSize(size_t typeId) const {
            auto it = sizes.find(typeId);
            if (it != sizes.end()) {
                return it->second;
            }
            return sizeof(void*); // Fallback to pointer size
        }

        const char* getComponentName(size_t typeId) const {
            auto it = names.find(typeId);
            if (it != names.end()) {
                return it->second;
            }
            return "Unknown";
        }

        size_t getTotalComponentSize(const std::bitset<MAX_COMPONENTS>& signature) const {
            size_t totalSize = 0;
            for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                if (signature.test(i)) {
                    totalSize += getComponentSize(i);
                }
            }
            return totalSize;
        }

    private:
        ComponentRegistry() = default;
        std::unordered_map<size_t, size_t> sizes;
        std::unordered_map<size_t, const char*> names;
    };

    // Chunk represents a fixed-size block of memory for component data
    class Chunk {
    public:
        Archetype* archetype;
        std::vector<uint8_t> data;  // Fixed-size chunk data
        size_t usedSize = 0;
        size_t entityCount = 0;

        Chunk(Archetype* arch) : archetype(arch) {
            data.resize(CHUNK_SIZE);  // Allocate exactly 16KB
        }

        bool hasSpace() const {
            return entityCount < getMaxEntities();
        }

        template<typename T>
        T* getComponent(size_t index) {
            return reinterpret_cast<T*>(data.data() + index * componentSize<T>);
        }

        size_t getMaxEntities() const;
    };

    // Archetype represents a unique combination of components
    class Archetype {
    public:
        std::bitset<MAX_COMPONENTS> signature;  // Component signature
        std::vector<size_t> entityIds;  // Entity IDs in this archetype
        std::vector<std::unique_ptr<Chunk>> chunks;  // Owned chunks
        size_t entityCount = 0;

        Archetype(const std::bitset<MAX_COMPONENTS>& sig) : signature(sig) {
            entityIds.reserve(INITIAL_ENTITY_CAPACITY);
        }

        template<typename T>
        T* getComponent(size_t index) {
            size_t chunkIndex = index / chunks[0]->getMaxEntities();
            size_t entityIndex = index % chunks[0]->getMaxEntities();
            return chunks[chunkIndex]->getComponent<T>(entityIndex);
        }

        void addEntity(size_t entityId) {
            entityIds.push_back(entityId);
            entityCount++;
        }

        void removeEntity(size_t index) {
            // Move last entity to this position
            if (index < entityCount - 1) {
                entityIds[index] = entityIds[entityCount - 1];
                
                // Calculate chunk and entity indices
                size_t maxEntitiesPerChunk = chunks[0]->getMaxEntities();
                size_t oldChunkIndex = index / maxEntitiesPerChunk;
                size_t oldEntityIndex = index % maxEntitiesPerChunk;
                size_t lastChunkIndex = (entityCount - 1) / maxEntitiesPerChunk;
                size_t lastEntityIndex = (entityCount - 1) % maxEntitiesPerChunk;

                // Move component data
                for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                    if (signature.test(i)) {
                        size_t compSize = ComponentRegistry::getInstance().getComponentSize(i);
                        uint8_t* oldData = chunks[oldChunkIndex]->data.data() + oldEntityIndex * compSize;
                        uint8_t* lastData = chunks[lastChunkIndex]->data.data() + lastEntityIndex * compSize;
                        std::memcpy(oldData, lastData, compSize);
                    }
                }
            }
            entityCount--;
        }

        Chunk* getOrCreateChunk(Ecs* ecs);
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
            // Register components if not already registered
            (ComponentRegistry::getInstance().registerComponent<Components>(), ...);

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
            Chunk* chunk = archetype->getOrCreateChunk(this);
            size_t chunkIndex = std::distance(archetype->chunks.begin(),
                std::find_if(archetype->chunks.begin(), archetype->chunks.end(),
                    [chunk](const auto& c) { return c.get() == chunk; }));

            // Create entity
            size_t entityIndex = chunk->entityCount;
            Entity entity(nextEntityId++, archetype, chunkIndex, entityIndex);
            entities.push_back(entity);

            // Initialize components with initializers
            archetype->addEntity(entity.id);
            (initializeComponent(archetype, entityIndex, initializers), ...);

            // Update chunk usage
            chunk->entityCount++;
            chunk->usedSize = chunk->entityCount * ComponentRegistry::getInstance().getTotalComponentSize(archetype->signature);

            return entity;
        }

        template<typename T>
        T* getComponent(const Entity& entity) {
            return entity.archetype->getComponent<T>(entity.entityIndex);
        }

        void destroyEntity(const Entity& entity) {
            // Remove from archetype
            entity.archetype->removeEntity(entity.entityIndex);
            
            // Update chunk usage
            Chunk* chunk = entity.archetype->chunks[entity.chunkIndex].get();
            chunk->entityCount--;
            chunk->usedSize = chunk->entityCount * ComponentRegistry::getInstance().getTotalComponentSize(entity.archetype->signature);
            
            // Optimize chunk usage after entity removal
            if (entity.archetype->chunks.size() > 1) {
                // If this chunk is not the last one and is not full, redistribute entities
                if (entity.chunkIndex < entity.archetype->chunks.size() - 1) {
                    Chunk* currentChunk = chunk;
                    Chunk* lastChunk = entity.archetype->chunks.back().get();
                    size_t maxEntitiesPerChunk = currentChunk->getMaxEntities();
                    
                    // If current chunk has space and last chunk has entities, redistribute
                    if (currentChunk->entityCount < maxEntitiesPerChunk && lastChunk->entityCount > 0) {
                        size_t entitiesToMove = std::min(
                            maxEntitiesPerChunk - currentChunk->entityCount,
                            lastChunk->entityCount
                        );

                        // Move entities from last chunk to current chunk
                        for (size_t j = 0; j < entitiesToMove; ++j) {
                            size_t sourceIndex = lastChunk->entityCount - entitiesToMove + j;
                            size_t targetIndex = currentChunk->entityCount + j;

                            // Get the entity ID being moved
                            size_t movedEntityId = entity.archetype->entityIds[entity.archetype->entityCount - entitiesToMove + j];

                            // Move each component
                            for (size_t comp = 0; comp < MAX_COMPONENTS; ++comp) {
                                if (entity.archetype->signature.test(comp)) {
                                    size_t compSize = ComponentRegistry::getInstance().getComponentSize(comp);
                                    uint8_t* sourceData = lastChunk->data.data() + sourceIndex * compSize;
                                    uint8_t* targetData = currentChunk->data.data() + targetIndex * compSize;
                                    std::memcpy(targetData, sourceData, compSize);
                                }
                            }

                            // Update the entity reference in the entities list
                            auto& entityRef = *std::find_if(entities.begin(), entities.end(),
                                [&](Entity& e) { return e.id == movedEntityId; });
                            entityRef.chunkIndex = entity.chunkIndex;
                            entityRef.entityIndex = targetIndex;
                        }

                        // Update entity counts and sizes
                        currentChunk->entityCount += entitiesToMove;
                        currentChunk->usedSize = currentChunk->entityCount * 
                            ComponentRegistry::getInstance().getTotalComponentSize(entity.archetype->signature);
                        
                        lastChunk->entityCount -= entitiesToMove;
                        lastChunk->usedSize = lastChunk->entityCount * 
                            ComponentRegistry::getInstance().getTotalComponentSize(entity.archetype->signature);

                        // If the last chunk is now empty, remove it
                        if (lastChunk->entityCount == 0) {
                            entity.archetype->chunks.pop_back();
                        }
                    }
                }
            }
            
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
            // Register component if not already registered
            ComponentRegistry::getInstance().registerComponent<T>();

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
            Chunk* newChunk = newArchetype->getOrCreateChunk(this);
            size_t newChunkIndex = std::distance(newArchetype->chunks.begin(),
                std::find_if(newArchetype->chunks.begin(), newArchetype->chunks.end(),
                    [newChunk](const auto& c) { return c.get() == newChunk; }));

            // Copy all components to new archetype
            size_t newEntityIndex = newChunk->entityCount;
            newArchetype->addEntity(entity.id);

            // Copy existing components
            for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                if (entity.archetype->signature.test(i)) {
                    size_t compSize = ComponentRegistry::getInstance().getComponentSize(i);
                    uint8_t* oldData = entity.archetype->chunks[entity.chunkIndex]->data.data() + entity.entityIndex * compSize;
                    uint8_t* newData = newChunk->data.data() + newEntityIndex * compSize;
                    std::memcpy(newData, oldData, compSize);
                }
            }

            // Initialize new component with initializer and get reference
            T& newComponent = *new (newArchetype->getComponent<T>(newEntityIndex)) T(initializer);

            // Update chunk usage
            newChunk->entityCount++;
            newChunk->usedSize = newChunk->entityCount * ComponentRegistry::getInstance().getTotalComponentSize(newArchetype->signature);

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
            Chunk* newChunk = newArchetype->getOrCreateChunk(this);
            size_t newChunkIndex = std::distance(newArchetype->chunks.begin(),
                std::find_if(newArchetype->chunks.begin(), newArchetype->chunks.end(),
                    [newChunk](const auto& c) { return c.get() == newChunk; }));

            // Copy all components except the one being removed
            size_t newEntityIndex = newChunk->entityCount;
            newArchetype->addEntity(entity.id);

            // Copy existing components
            for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                if (entity.archetype->signature.test(i) && i != typeId<T>) {
                    size_t compSize = ComponentRegistry::getInstance().getComponentSize(i);
                    uint8_t* oldData = entity.archetype->chunks[entity.chunkIndex]->data.data() + entity.entityIndex * compSize;
                    uint8_t* newData = newChunk->data.data() + newEntityIndex * compSize;
                    std::memcpy(newData, oldData, compSize);
                }
            }

            // Update chunk usage
            newChunk->entityCount++;
            newChunk->usedSize = newChunk->entityCount * ComponentRegistry::getInstance().getTotalComponentSize(newArchetype->signature);

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

    // Implementation of Chunk::getMaxEntities
    inline size_t Chunk::getMaxEntities() const {
        if (archetype->signature.count() == 0) return 0;
        size_t totalComponentSize = ComponentRegistry::getInstance().getTotalComponentSize(archetype->signature);
        return CHUNK_SIZE / totalComponentSize;
    }

    
    inline Chunk* Archetype::getOrCreateChunk(Ecs* ecs) {
            // First check if the last chunk has space
            if (!chunks.empty() && chunks.back()->hasSpace()) {
                return chunks.back().get();
            }

            // If we have more than one chunk and the last one isn't full,
            // redistribute entities to fill gaps in previous chunks
            if (chunks.size() > 1 && !chunks.back()->hasSpace()) {
                size_t maxEntitiesPerChunk = chunks[0]->getMaxEntities();
                size_t lastChunkIndex = chunks.size() - 1;
                Chunk* lastChunk = chunks[lastChunkIndex].get();
                
                // Move entities from the last chunk to fill gaps in previous chunks
                while (lastChunk->entityCount > 0) {
                    bool moved = false;
                    for (size_t i = 0; i < lastChunkIndex; ++i) {
                        Chunk* targetChunk = chunks[i].get();
                        if (targetChunk->hasSpace()) {
                            // Calculate indices
                            size_t lastEntityIndex = lastChunk->entityCount - 1;
                            size_t targetEntityIndex = targetChunk->entityCount;
                            
                            // Move component data
                            for (size_t compIndex = 0; compIndex < MAX_COMPONENTS; ++compIndex) {
                                if (signature.test(compIndex)) {
                                    size_t compSize = ComponentRegistry::getInstance().getComponentSize(compIndex);
                                    uint8_t* sourceData = lastChunk->data.data() + lastEntityIndex * compSize;
                                    uint8_t* targetData = targetChunk->data.data() + targetEntityIndex * compSize;
                                    std::memcpy(targetData, sourceData, compSize);
                                }
                            }
                            
                            // Update entity reference in ECS
                            size_t entityId = entityIds[lastChunkIndex * maxEntitiesPerChunk + lastEntityIndex];
                            for (auto& entity : ecs->entities) {
                                if (entity.id == entityId) {
                                    entity.chunkIndex = i;
                                    entity.entityIndex = targetEntityIndex;
                                    break;
                                }
                            }
                            
                            // Update chunk counts
                            targetChunk->entityCount++;
                            targetChunk->usedSize += ComponentRegistry::getInstance().getTotalComponentSize(signature);
                            lastChunk->entityCount--;
                            lastChunk->usedSize -= ComponentRegistry::getInstance().getTotalComponentSize(signature);
                            
                            moved = true;
                            break;
                        }
                    }
                    
                    if (!moved) break;  // No more space in previous chunks
                }
                
                // Remove the last chunk if it's empty
                if (lastChunk->entityCount == 0) {
                    chunks.pop_back();
                }
            }
            
            // Create a new chunk if all existing chunks are full
            if (chunks.empty() || !chunks.back()->hasSpace()) {
                chunks.push_back(std::make_unique<Chunk>(this));
            }
            
            return chunks.back().get();
        }
} // namespace ecs