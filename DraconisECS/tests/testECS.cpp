#include "testECS.h"
#include <random>
#include <algorithm>
#include <numeric>

namespace test::ecs {

void createTestEntities(::ecs::Ecs& ecs) {
	// Create entities with different component combinations
	for (int i = 0; i < 10; ++i) {
		ecs.createEntity(Transform{static_cast<float>(i), 0.0f, 0.0f, 0.0f});
	}

	for (int i = 0; i < 5; ++i) {
		ecs.createEntity(
			Transform{static_cast<float>(i), 0.0f, 0.0f, 0.0f},
			Sprite{"sprite" + std::to_string(i) + ".png"}
		);
	}

	for (int i = 0; i < 3; ++i) {
		ecs.createEntity(
			Transform{static_cast<float>(i), 0.0f, 0.0f, 0.0f},
			Sprite{"sprite" + std::to_string(i) + ".png"},
			Physics{0.0f, 0.0f, 1.0f, false}
		);
	}
}

void createComplexEntities(::ecs::Ecs& ecs) {
	// Create entities with more complex component combinations
	for (int i = 0; i < 15; ++i) {
		ecs.createEntity(
			Transform{static_cast<float>(i), 0.0f, 0.0f, 0.0f},
			Sprite{"complex" + std::to_string(i) + ".png"},
			Physics{0.0f, 0.0f, 1.0f, false},
			Health{100, 100, false}
		);
	}

	for (int i = 0; i < 10; ++i) {
		ecs.createEntity(
			Transform{static_cast<float>(i), 0.0f, 0.0f, 0.0f},
			Sprite{"complex" + std::to_string(i) + ".png"},
			Physics{0.0f, 0.0f, 1.0f, false},
			Health{100, 100, false},
			AI{"aggressive", 15.0f, true}
		);
	}
}

void createPerformanceTestEntities(::ecs::Ecs& ecs) {
	// Create a large number of entities for performance testing
	for (int i = 0; i < 1000; ++i) {
		ecs.createEntity(
			Transform{static_cast<float>(i), 0.0f, 0.0f, 0.0f},
			Sprite{"perf" + std::to_string(i) + ".png"},
			Physics{0.0f, 0.0f, 1.0f, false}
		);
	}
}

void testRandomEntityOperations(::ecs::Ecs& ecs) {
	if (ecs.getEntities().empty()) {
		// If no entities exist, create some test entities first
		createComplexEntities(ecs);
	}

	// Create random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> opDist(0.0f, 1.0f);

	// Perform random operations on 10% of current entities
	size_t numOperations = ecs.getEntities().size() / 10;

	for (size_t i = 0; i < numOperations; ++i) {
		const auto& entities = ecs.getEntities();
		if (entities.empty()) break;  // Stop if no entities left

		// Pick a random entity
		std::uniform_int_distribution<size_t> entityDist(0, entities.size() - 1);
		const auto& entity = entities[entityDist(gen)];
		float operation = opDist(gen);

		if (operation < 0.3f) {
			// 30% chance to delete entity
			ecs.destroyEntity(entity);
		} else if (operation < 0.6f) {
			// 30% chance to add a random component
			const auto& signature = entity.getSignature();
			switch (std::uniform_int_distribution<>(0, 4)(gen)) {
				case 0:
					if (!signature[::ecs::typeId<Health>]) {
						ecs.addComponent(entity, Health{100, 100, false});
					}
					break;
				case 1:
					if (!signature[::ecs::typeId<AI>]) {
						ecs.addComponent(entity, AI{"random", 10.0f, false});
					}
					break;
				case 2:
					if (!signature[::ecs::typeId<Physics>]) {
						ecs.addComponent(entity, Physics{0.0f, 0.0f, 1.0f, false});
					}
					break;
				case 3:
					if (!signature[::ecs::typeId<Sprite>]) {
						ecs.addComponent(entity, Sprite{"random.png"});
					}
					break;
				case 4:
					if (!signature[::ecs::typeId<Transform>]) {
						ecs.addComponent(entity, Transform{0.0f, 0.0f, 0.0f, 0.0f});
					}
					break;
			}
		} else {
			// 40% chance to remove a random component
			std::vector<size_t> componentIndices;
			const auto& signature = entity.getSignature();
			for (size_t j = 0; j < ::ecs::MAX_COMPONENTS; ++j) {
				if (signature[j]) {
					componentIndices.push_back(j);
				}
			}
			if (!componentIndices.empty()) {
				size_t randomCompIndex = componentIndices[std::uniform_int_distribution<size_t>(0, componentIndices.size() - 1)(gen)];
				
				if (randomCompIndex == ::ecs::typeId<Health>) {
					ecs.removeComponent<Health>(entity);
				} else if (randomCompIndex == ::ecs::typeId<AI>) {
					ecs.removeComponent<AI>(entity);
				} else if (randomCompIndex == ::ecs::typeId<Physics>) {
					ecs.removeComponent<Physics>(entity);
				} else if (randomCompIndex == ::ecs::typeId<Sprite>) {
					ecs.removeComponent<Sprite>(entity);
				} else if (randomCompIndex == ::ecs::typeId<Transform>) {
					ecs.removeComponent<Transform>(entity);
				}
			}
		}
	}
}

} // namespace test::ecs 