#pragma once
#include "config/config.h"
#include "core/modules/editor.h"

namespace module::editor
{
class ConfigPanel : public EditorPanel
{
  public:
    void render() override;
    const char *getName() const override
    {
        return "Configuration";
    }

  private:
    void renderWindowConfig();
    void renderGraphicsConfig();
    bool isDirty = false;
};
} // namespace module::editor