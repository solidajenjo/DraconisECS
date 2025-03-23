#pragma once
#include <memory>
#include "core/modules/editor.h"
#include "core/modules/render.h"

namespace module::editor
{
class RenderPanel : public EditorPanel
{
public:
	explicit RenderPanel( module::Render* renderModule );
	void render() override;
	const char* getName() const override
	{
		return "Scene View";
	}

private:
	module::Render* m_RenderModule;
	ImVec2 m_LastSize;
};
}  // namespace module::editor