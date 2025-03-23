#pragma once
#include <imgui.h>
#include <bitset>
#include <cstddef>
#include <string>
#include <vector>
#include "app.h"
#include "core/ecs/ecs.h"
#include "core/modules/editor.h"

namespace module::editor
{
class ArchetypePanel : public EditorPanel
{
public:
	ArchetypePanel() = default;
	const char* getName() const override;
	void render() override;

private:
	bool isOpen = true;
};
}  // namespace module::editor