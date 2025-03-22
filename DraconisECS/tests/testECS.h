#pragma once
#include "core/ecs/ecs.h"
#include <string>

namespace test::ecs {

// Mock components
struct Transform {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float rotation = 0.0f;
};

struct Sprite {
	std::string texturePath;
	int width = 32;
	int height = 32;
};

struct Physics {
	float velocityX = 0.0f;
	float velocityY = 0.0f;
	float mass = 1.0f;
	bool isStatic = false;
};

struct Health {
	int maxHealth = 100;
	int currentHealth = 100;
	bool isInvulnerable = false;
};

struct AI {
	std::string behaviorType;
	float detectionRange = 10.0f;
	bool isAggressive = false;
};

// Test functions
void createTestEntities(::ecs::Ecs& ecs);
void createComplexEntities(::ecs::Ecs& ecs);
void createPerformanceTestEntities(::ecs::Ecs& ecs);
void testRandomEntityOperations(::ecs::Ecs& ecs);
void testComponentNames(::ecs::Ecs& ecs);

} // namespace test::ecs     