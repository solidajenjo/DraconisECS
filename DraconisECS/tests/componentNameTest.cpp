#include "testECS.h"
#include <iostream>

namespace test::ecs {

void testComponentNames(::ecs::Ecs& ecs) {
    // Create an entity with multiple components
    auto entity = ecs.createEntity(
        Transform{},
        Sprite{"test.png"},
        Physics{},
        Health{100}
    );

    // Get the archetype signature
    auto& signature = entity.getArchetype()->getSignature();

    // Print component names
    std::cout << "Entity components:\n";
    for (size_t i = 0; i < ::ecs::MAX_COMPONENTS; ++i) {
        if (signature.test(i)) {
            const char* name = ::ecs::ComponentRegistry::getInstance().getComponentName(i);
            size_t size = ::ecs::ComponentRegistry::getInstance().getComponentSize(i);
            std::cout << "- " << name << " (" << size << " bytes)\n";
        }
    }
}

} // namespace test::ecs 