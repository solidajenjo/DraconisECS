#pragma once
#include "core/modules/editor.h"
#include "core/ecs.h"
#include "app.h"
#include <imgui.h>
#include <vector>
#include <string>
#include <bitset>
#include <cstddef>

namespace module::editor {

class ArchetypePanel : public EditorPanel {
public:
    ArchetypePanel() = default;
    const char* getName() const override;
    void render() override;

private:
    bool isOpen = true;
};

} // namespace module::editor 