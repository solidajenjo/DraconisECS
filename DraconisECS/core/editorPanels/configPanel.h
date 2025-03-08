#pragma once
#include "Config.h"
#include "core/modules/editor.h"

namespace module::editor
{
class ConfigPanel : public EditorPanel
{
  public:
    void render() override;

  private:
    void renderWindowConfig();
    void renderGraphicsConfig();
    bool isDirty = false;
};
} // namespace module::editor