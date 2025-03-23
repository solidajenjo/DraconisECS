#pragma once
#include <string>
#include "core/ecs/ecs.h"

namespace test::ecs
{

// Test functions
void createTestEntities( ::ecs::Ecs& ecs );
void createComplexEntities( ::ecs::Ecs& ecs );
void createPerformanceTestEntities( ::ecs::Ecs& ecs );
void testRandomEntityOperations( ::ecs::Ecs& ecs );

}  // namespace test::ecs