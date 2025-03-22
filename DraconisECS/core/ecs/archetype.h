#pragma once
#include <vector>
#include <memory>
#include <bitset>
#include <cassert>
#include "chunk.h"
#include "componentRegistry.h"
#include "ecsGlobals.h"

namespace ecs {
    // Forward declarations
    class Ecs;

    // Archetype represents a unique combination of components
    class Archetype {
    public:
        explicit Archetype(const std::bitset<MAX_COMPONENTS>& sig);
        ~Archetype();

        const std::bitset<MAX_COMPONENTS>& getSignature() const;
        size_t getEntityCount() const;
        const std::vector<size_t>& getEntityIds() const;
        const std::vector<std::unique_ptr<Chunk>>& getChunks() const;
        std::vector<std::unique_ptr<Chunk>>& getMutableChunks();
        Chunk& getOrCreateChunk();

        template<typename T>
        T& getComponent(size_t index);

        template<typename T>
        const T& getComponent(size_t index) const;

    private:
        std::bitset<MAX_COMPONENTS> signature;  // Component signature
        std::vector<size_t> entityIds;  // Entity IDs in this archetype
        std::vector<std::unique_ptr<Chunk>> chunks;  // Owned chunks
        size_t entityCount = 0;

        // Make Ecs, Chunk, and ArchetypeView friends to allow them to access private methods
        friend class Ecs;
        friend class Chunk;
        friend class ArchetypeView;

        void addEntity(size_t entityId);
        void removeEntity(size_t index);
        Chunk& getOrCreateChunk(Ecs& ecs);
    };

    // ArchetypeView provides a read-only interface for UI purposes
	class ArchetypeView {
	public:
		ArchetypeView(const Archetype& archetype) : archetype(archetype) {}

		const std::bitset<MAX_COMPONENTS>& getSignature() const { return archetype.getSignature(); }
		size_t getEntityCount() const { return archetype.getEntityCount(); }
		const std::vector<size_t>& getEntityIds() const { return archetype.getEntityIds(); }
		const std::vector<std::unique_ptr<Chunk>>& getChunks() const { return archetype.getChunks(); }

	private:
		const Archetype& archetype;
		friend class Ecs;  // Allow Ecs to create ArchetypeView instances
	};
} 