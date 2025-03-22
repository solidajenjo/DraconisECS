#include "archetypePanel.h"
#include "tests/testECS.h"
#include <imgui.h>

namespace module::editor {

const char* ArchetypePanel::getName() const {
    return "Archetype Viewer";
}

void ArchetypePanel::render() {
    if (!isOpen) return;

    ImGui::Begin(getName(), &isOpen);

    // Get reference to ECS system
    auto& ecs = app::appInstance.getEcs();

    // Add test buttons at the top
    if (ImGui::CollapsingHeader("Test Controls")) {
        if (ImGui::Button("Create Basic Test Entities")) {
            test::createTestEntities(ecs);
        }
        if (ImGui::Button("Create Complex Entities")) {
            test::createComplexEntities(ecs);
        }
        if (ImGui::Button("Create Performance Test Entities")) {
            test::createPerformanceTestEntities(ecs);
        }
        ImGui::Separator();
    }

    // Display total entity count
    ImGui::Text("Total Entities: %zu", ecs.entities.size());

    // Display archetypes
    for (const auto& [signature, archetype] : ecs.archetypes) {
        // Push unique ID for this archetype
        ImGui::PushID(static_cast<int>(reinterpret_cast<uintptr_t>(archetype.get())));
        
        if (ImGui::CollapsingHeader(("Archetype " + std::to_string(reinterpret_cast<uintptr_t>(archetype.get()))).c_str())) {
            // Display entity count
            ImGui::Text("Entities: %zu", archetype->entityCount);

            // Display chunks
            if (ImGui::TreeNode("Chunks")) {
                for (size_t i = 0; i < archetype->chunks.size(); ++i) {
                    const auto& chunk = archetype->chunks[i];
                    // Push unique ID for this chunk
                    ImGui::PushID(static_cast<int>(i));
                    
                    if (ImGui::TreeNode(("Chunk " + std::to_string(i)).c_str())) {
                        // Display chunk usage
                        ImGui::Text("Used Size: %zu/%d", chunk->usedSize, ecs::CHUNK_SIZE);
                        
                        // Display memory layout
                        if (ImGui::TreeNode("Memory Layout")) {
                            size_t totalSize = 0;
                            for (size_t compIdx = 0; compIdx < ecs::MAX_COMPONENTS; ++compIdx) {
                                if (signature[compIdx]) {
                                    // Get component size (this is a simplified version)
                                    size_t compSize = sizeof(void*); // We'll need to implement proper component size tracking
                                    totalSize += compSize;
                                    ImGui::Text("Component %zu: %zu bytes", compIdx, compSize);
                                }
                            }
                            ImGui::Text("Total Size: %zu bytes", totalSize);
                            ImGui::TreePop();
                        }

                        // Display entities in chunk
                        if (ImGui::TreeNode("Entities")) {
                            for (size_t j = 0; j < chunk->usedSize; ++j) {
                                // Push unique ID for each entity
                                ImGui::PushID(static_cast<int>(j));
                                ImGui::Text("Entity %zu", archetype->entityIds[j]);
                                ImGui::PopID();
                            }
                            ImGui::TreePop();
                        }

                        ImGui::TreePop();
                    }
                    
                    ImGui::PopID(); // Pop chunk ID
                }
                ImGui::TreePop();
            }

            // Display memory statistics
            if (ImGui::TreeNode("Memory Statistics")) {
                size_t totalMemory = 0;
                for (size_t i = 0; i < ecs::MAX_COMPONENTS; ++i) {
                    if (signature[i]) {
                        size_t compSize = sizeof(void*); // Simplified
                        totalMemory += compSize * archetype->entityCount;
                    }
                }
                ImGui::Text("Total Memory Used: %zu bytes", totalMemory);
                ImGui::Text("Average Memory per Entity: %.2f bytes", 
                          archetype->entityCount > 0 ? static_cast<float>(totalMemory) / archetype->entityCount : 0.0f);
                ImGui::TreePop();
            }
        }
        
        ImGui::PopID(); // Pop archetype ID
    }

    ImGui::End();
}

} // namespace module::editor 