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

    // Add helper function to get archetype name
    auto getArchetypeName = [](const std::bitset<::ecs::MAX_COMPONENTS>& signature) -> std::string {
        std::vector<std::string> components;
        for (size_t i = 0; i < ::ecs::MAX_COMPONENTS; ++i) {
            if (signature.test(i)) {
                components.push_back(::ecs::ComponentRegistry::getInstance().getComponentName(i));
            }
        }
        if (components.empty()) return "Empty";
        std::string name = components[0];
        for (size_t i = 1; i < components.size(); ++i) {
            name += " + " + components[i];
        }
        return name;
    };

    // Add test buttons at the top
    if (ImGui::CollapsingHeader("Test Controls")) {
        if (ImGui::Button("Create Basic Test Entities")) {
            test::ecs::createTestEntities(ecs);
        }
        if (ImGui::Button("Create Complex Entities")) {
            test::ecs::createComplexEntities(ecs);
        }
        if (ImGui::Button("Create Performance Test Entities")) {
            test::ecs::createPerformanceTestEntities(ecs);
        }
        if (ImGui::Button("Test Random Entity Operations")) {
            test::ecs::testRandomEntityOperations(ecs);
        }
        if (ImGui::Button("Test Component Names")) {
            test::ecs::testComponentNames(ecs);
        }
        ImGui::Separator();
    }

    // Display total entity count
    ImGui::Text("Total Entities: %zu", ecs.entities.size());
    ImGui::Separator();

    // Display archetypes in a table
    if (ImGui::BeginTable("Archetypes", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Archetype");
        ImGui::TableSetupColumn("Entity Count");
        ImGui::TableSetupColumn("Memory Usage");
        ImGui::TableSetupColumn("Chunks");
        ImGui::TableHeadersRow();

        for (const auto& [signature, archetype] : ecs.archetypes) {
            ImGui::TableNextRow();
            
            // Archetype name column
            ImGui::TableNextColumn();
            std::string archetypeName = getArchetypeName(signature);
            ImGui::Text("%s", archetypeName.c_str());

            // Entity count column
            ImGui::TableNextColumn();
            ImGui::Text("%zu", archetype->entityCount);

            // Memory usage column
            ImGui::TableNextColumn();
            size_t totalSize = 0;
            for (size_t i = 0; i < ::ecs::MAX_COMPONENTS; ++i) {
                if (signature.test(i)) {
                    totalSize += ::ecs::ComponentRegistry::getInstance().getComponentSize(i);
                }
            }
            size_t totalMemory = totalSize * archetype->entityCount;
            ImGui::Text("%zu bytes", totalMemory);

            // Chunks column
            ImGui::TableNextColumn();
            if (ImGui::BeginTable(("Chunks_" + archetypeName).c_str(), 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Index");
                ImGui::TableSetupColumn("Usage");
                ImGui::TableSetupColumn("Memory");
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < archetype->chunks.size(); ++i) {
                    const auto& chunk = archetype->chunks[i];
                    ImGui::TableNextRow();
                    
                    // Chunk index
                    ImGui::TableNextColumn();
                    ImGui::Text("%zu", i);

                    // Usage
                    ImGui::TableNextColumn();
                    float entityRatio = static_cast<float>(chunk->entityCount) / chunk->getMaxEntities();
                    ImGui::ProgressBar(entityRatio, ImVec2(-1, 0), "");
                    ImGui::SameLine();
                    ImGui::Text("%zu/%zu", chunk->entityCount, chunk->getMaxEntities());

                    // Memory
                    ImGui::TableNextColumn();
                    float memoryRatio = static_cast<float>(chunk->usedSize) / (chunk->getMaxEntities() * totalSize);
                    ImGui::ProgressBar(memoryRatio, ImVec2(-1, 0), "");
                    ImGui::SameLine();
                    ImGui::Text("%zu/%zu", chunk->usedSize, chunk->getMaxEntities() * totalSize);
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndTable();
    }

    // Display component details in a separate table
    ImGui::Separator();
    ImGui::Text("Component Details");
    if (ImGui::BeginTable("Components", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Component");
        ImGui::TableSetupColumn("Size");
        ImGui::TableSetupColumn("Count");
        ImGui::TableHeadersRow();

        std::unordered_map<size_t, size_t> componentCounts;
        for (const auto& [signature, archetype] : ecs.archetypes) {
            for (size_t i = 0; i < ::ecs::MAX_COMPONENTS; ++i) {
                if (signature.test(i)) {
                    componentCounts[i] += archetype->entityCount;
                }
            }
        }

        for (const auto& [typeId, count] : componentCounts) {
            ImGui::TableNextRow();
            
            // Component name
            ImGui::TableNextColumn();
            ImGui::Text("%s", ::ecs::ComponentRegistry::getInstance().getComponentName(typeId));

            // Component size
            ImGui::TableNextColumn();
            ImGui::Text("%zu bytes", ::ecs::ComponentRegistry::getInstance().getComponentSize(typeId));

            // Component count
            ImGui::TableNextColumn();
            ImGui::Text("%zu", count);
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

} // namespace module::editor 