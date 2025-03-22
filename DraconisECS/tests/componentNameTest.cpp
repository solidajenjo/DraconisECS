#include "testECS.h"
#include <iostream>

namespace test::ecs {

void testComponentNames(::ecs::Ecs& ecs) {
    // Create an entity with multiple components
    auto entity = ecs.createEntity(
        Transform{0.0f, 0.0f, 0.0f, 0.0f},
        Sprite{"test.png"},
        Physics{0.0f, 0.0f, 1.0f, false},
        Health{100, 100, false}
    );

    // Get the component signature
    const auto& signature = entity.getSignature();

    // Print component names and sizes
    std::cout << "Entity components:\n";
    for (size_t i = 0; i < ::ecs::MAX_COMPONENTS; ++i) {
        if (signature[i]) {
            std::cout << "- " << ::ecs::ComponentRegistry::getInstance().getComponentName(i)
                      << " (size: " << ::ecs::ComponentRegistry::getInstance().getComponentSize(i) << " bytes)\n";
        }
    }
}

} // namespace test::ecs 