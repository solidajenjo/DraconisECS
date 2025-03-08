#pragma once
#include "core/modules/editor.h"

namespace module::editor
{
class RenderPanel : public EditorPanel
{
  public:
    void render() override;
};
} // namespace module::editor