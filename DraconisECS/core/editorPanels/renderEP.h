#pragma once
#include "modules/editor.h"

namespace editorPanel
{ 
	class RenderEP : public module::editor::EditorPanel
	{
	public:
		void render() const override;
	};
}