#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cstdint>
#include <tuple>
#include "chunk.h"
#include "archetype.h"
#include "componentRegistry.h"
#include "ecsGlobals.h"
#include <cassert>

namespace ecs
{
	// Forward declarations
	class Ecs;

	// Entity represents a unique ID with its archetype
	class Entity {
	public:
		Entity(size_t entityId, const std::bitset<MAX_COMPONENTS>& sig, size_t chunkIdx, size_t entityIdx)
			: id(entityId), signature(sig), chunkIndex(chunkIdx), entityIndex(entityIdx) {}

		size_t getId() const { return id; }
		const std::bitset<MAX_COMPONENTS>& getSignature() const { return signature; }
		size_t getChunkIndex() const { return chunkIndex; }
		size_t getEntityIndex() const { return entityIndex; }

		void setChunkIndex(size_t idx) { chunkIndex = idx; }
		void setEntityIndex(size_t idx) { entityIndex = idx; }
		void setSignature(const std::bitset<MAX_COMPONENTS>& sig) { signature = sig; }

	private:
		size_t id;
		std::bitset<MAX_COMPONENTS> signature;  // Store component signature instead of archetype ID
		size_t chunkIndex;
		size_t entityIndex;
	};

	// Ecs manages the entire ECS system
	class Ecs {
	public:
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
			Chunk& chunk = archetype->getOrCreateChunk();
			size_t chunkIndex = std::distance(archetype->getChunks().begin(),
				std::find_if(archetype->getChunks().begin(), archetype->getChunks().end(),
					[&chunk](const auto& c) { return c.get() == &chunk; }));

			// Create entity
			size_t entityIndex = chunk.getEntityCount();
			Entity entity(nextEntityId++, signature, chunkIndex, entityIndex);
			entities.push_back(entity);

			// Initialize components with initializers
			archetype->addEntity(entity.getId());
			(initializeComponent(archetype, entityIndex, initializers), ...);

			// Update chunk usage
			chunk.incrementEntityCount();
			chunk.setUsedSize(chunk.getEntityCount() * ComponentRegistry::getInstance().getTotalComponentSize(archetype->getSignature()));

			return entity;
		}

		template<typename T>
		T* getComponent(const Entity& entity) {
			Archetype* archetype = getArchetypeBySignature(entity.getSignature());
			if (!archetype) return nullptr;
			return archetype->getComponent<T>(entity.getEntityIndex());
		}

		void destroyEntity(const Entity& entity) {
			Archetype* archetype = getArchetypeBySignature(entity.getSignature());
			if (!archetype) return;

			// Remove from archetype
			archetype->removeEntity(entity.getEntityIndex());
			
			// Optimize chunk usage after entity removal
			if (archetype->getChunks().size() > 1) {
				// If this chunk is not the last one and is not full, redistribute entities
				if (entity.getChunkIndex() < archetype->getChunks().size() - 1) {
					Chunk& currentChunk = *archetype->getChunks()[entity.getChunkIndex()];
					Chunk& lastChunk = *archetype->getChunks().back();
					size_t maxEntitiesPerChunk = currentChunk.getMaxEntities();
					
					// If current chunk has space and last chunk has entities, redistribute
					if (currentChunk.getEntityCount() < maxEntitiesPerChunk && lastChunk.getEntityCount() > 0) {
						size_t entitiesToMove = std::min(
							maxEntitiesPerChunk - currentChunk.getEntityCount(),
							lastChunk.getEntityCount()
						);

						// Move entities from last chunk to current chunk
						for (size_t j = 0; j < entitiesToMove; ++j) {
							size_t sourceIndex = lastChunk.getEntityCount() - j - 1;  // Start from last entity
							size_t targetIndex = currentChunk.getEntityCount();  // Place at first available spot

							// Get the entity ID being moved
							size_t movedEntityId = archetype->getEntityIds()[
								archetype->getEntityCount() - j - 1];

							// Move each component
							for (size_t comp = 0; comp < MAX_COMPONENTS; ++comp) {
								if (archetype->getSignature().test(comp)) {
									size_t compSize = ComponentRegistry::getInstance().getComponentSize(comp);
									currentChunk.copyComponentDataFromChunk(
										lastChunk,
										sourceIndex,
										targetIndex,
										compSize
									);
								}
							}

							// Update the entity reference in the entities list
							updateEntityReference(movedEntityId, entity.getSignature(), entity.getChunkIndex(), targetIndex);

							// Update counts after each move
							currentChunk.incrementEntityCount();
							currentChunk.setUsedSize(currentChunk.getEntityCount() * 
								ComponentRegistry::getInstance().getTotalComponentSize(archetype->getSignature()));
							
							lastChunk.decrementEntityCount();
							lastChunk.setUsedSize(lastChunk.getEntityCount() * 
								ComponentRegistry::getInstance().getTotalComponentSize(archetype->getSignature()));
						}

						// If the last chunk is now empty, remove it
						if (lastChunk.getEntityCount() == 0) {
							archetype->getMutableChunks().pop_back();
						}
					}
				}
			}
			
			// Remove from entities list
			auto it = std::find_if(entities.begin(), entities.end(),
				[&](const Entity& e) { return e.getId() == entity.getId(); });
			if (it != entities.end()) {
				entities.erase(it);
			}
		}

		// Add getters for testing and UI
		const std::vector<Entity>& getEntities() const { return entities; }
		const std::unordered_map<std::bitset<MAX_COMPONENTS>, std::unique_ptr<Archetype>>& getArchetypes() const { return archetypes; }

		template<typename T>
		void addComponent(const Entity& entity, const T& initializer) {
			// Create new signature with added component
			std::bitset<MAX_COMPONENTS> newSignature = entity.getSignature();
			newSignature.set(typeId<T>);

			// Get or create new archetype
			auto it = archetypes.find(newSignature);
			Archetype* newArchetype;
			if (it == archetypes.end()) {
				newArchetype = archetypes.emplace(newSignature, std::make_unique<Archetype>(newSignature)).first->second.get();
			} else {
				newArchetype = it->second.get();
			}

			// Get or create chunk in new archetype
			Chunk& newChunk = newArchetype->getOrCreateChunk();
			size_t newChunkIndex = std::distance(newArchetype->getChunks().begin(),
				std::find_if(newArchetype->getChunks().begin(), newArchetype->getChunks().end(),
					[&newChunk](const auto& c) { return c.get() == &newChunk; }));
			size_t newEntityIndex = newChunk.getEntityCount();

			// Copy existing components
			Archetype* oldArchetype = getArchetypeBySignature(entity.getSignature());
			if (oldArchetype) {
				for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
					if (entity.getSignature().test(i)) {
						size_t compSize = ComponentRegistry::getInstance().getComponentSize(i);
						newChunk.copyComponentDataFromChunk(
							*oldArchetype->getChunks()[entity.getChunkIndex()],
							entity.getEntityIndex(),
							newEntityIndex,
							compSize
						);
					}
				}
			}

			// Initialize new component
			newChunk.createComponent(newEntityIndex, initializer);

			// Update entity reference
			updateEntityReference(entity.getId(), newSignature, newChunkIndex, newEntityIndex);

			// Update chunk usage
			newChunk.incrementEntityCount();
			newChunk.setUsedSize(newChunk.getEntityCount() * 
				ComponentRegistry::getInstance().getTotalComponentSize(newSignature));
		}

		template<typename T>
		void removeComponent(const Entity& entity) {
			// Create new signature without the component
			std::bitset<MAX_COMPONENTS> newSignature = entity.getSignature();
			newSignature.reset(typeId<T>);

			// Get or create new archetype
			auto it = archetypes.find(newSignature);
			Archetype* newArchetype;
			if (it == archetypes.end()) {
				newArchetype = archetypes.emplace(newSignature, std::make_unique<Archetype>(newSignature)).first->second.get();
			} else {
				newArchetype = it->second.get();
			}

			// Get or create chunk in new archetype
			Chunk& newChunk = newArchetype->getOrCreateChunk();
			size_t newChunkIndex = std::distance(newArchetype->getChunks().begin(),
				std::find_if(newArchetype->getChunks().begin(), newArchetype->getChunks().end(),
					[&newChunk](const auto& c) { return c.get() == &newChunk; }));
			size_t newEntityIndex = newChunk.getEntityCount();

			// Copy existing components except the one being removed
			Archetype* oldArchetype = getArchetypeBySignature(entity.getSignature());
			if (oldArchetype) {
				for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
					if (entity.getSignature().test(i) && i != typeId<T>) {
						size_t compSize = ComponentRegistry::getInstance().getComponentSize(i);
						newChunk.copyComponentDataFromChunk(
							*oldArchetype->getChunks()[entity.getChunkIndex()],
							entity.getEntityIndex(),
							newEntityIndex,
							compSize
						);
					}
				}
			}

			// Update entity reference
			updateEntityReference(entity.getId(), newSignature, newChunkIndex, newEntityIndex);

			// Update chunk usage
			newChunk.incrementEntityCount();
			newChunk.setUsedSize(newChunk.getEntityCount() * 
				ComponentRegistry::getInstance().getTotalComponentSize(newSignature));
		}

		// Update getters for UI to use ArchetypeView
		std::vector<ArchetypeView> getArchetypeViews() const {
			std::vector<ArchetypeView> views;
			views.reserve(archetypes.size());
			for (const auto& [_, archetype] : archetypes) {
				views.emplace_back(*archetype);
			}
			return views;
		}

	private:
		std::unordered_map<std::bitset<MAX_COMPONENTS>, std::unique_ptr<Archetype>> archetypes;
		std::vector<Entity> entities;
		size_t nextEntityId = 0;

		// Make Archetype a friend to allow access to private methods
		friend class Archetype;

		// Helper function to initialize a single component with its initializer
		template<typename T>
		void initializeComponent(Archetype* archetype, size_t entityIndex, const T& initializer) {
			// Get the chunk that this entity belongs to
			Chunk& chunk = archetype->getOrCreateChunk();
			chunk.createComponent(entityIndex, initializer);
		}

		// Helper function to get archetype by signature
		Archetype* getArchetypeBySignature(const std::bitset<MAX_COMPONENTS>& signature) {
			auto it = archetypes.find(signature);
			return it != archetypes.end() ? it->second.get() : nullptr;
		}

		// Helper function to get archetype by signature (const version)
		const Archetype* getArchetypeBySignature(const std::bitset<MAX_COMPONENTS>& signature) const {
			auto it = archetypes.find(signature);
			return it != archetypes.end() ? it->second.get() : nullptr;
		}

		// Update entity reference with assertions for development-time checks
		bool updateEntityReference(size_t entityId, const std::bitset<MAX_COMPONENTS>& signature, size_t chunkIndex, size_t entityIndex) {
			Archetype* archetype = getArchetypeBySignature(signature);
			assert(archetype != nullptr);
			assert(chunkIndex < archetype->getChunks().size());
			assert(entityIndex < archetype->getChunks()[chunkIndex]->getMaxEntities());

			auto it = std::find_if(entities.begin(), entities.end(),
				[entityId](const Entity& e) { return e.getId() == entityId; });
			assert(it != entities.end());

			it->setSignature(signature);
			it->setChunkIndex(chunkIndex);
			it->setEntityIndex(entityIndex);
			
			return true;
		}
	};
} // namespace ecs