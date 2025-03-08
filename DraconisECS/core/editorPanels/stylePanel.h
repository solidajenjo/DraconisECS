#pragma once

#include "core/modules/editor.h"
#include <memory>

namespace module::editor
{

class StylePanel : public EditorPanel
{
  public:
    StylePanel();
    virtual ~StylePanel();
    const char *getName() const override;
    void render() override;
};

} // namespace module::editor