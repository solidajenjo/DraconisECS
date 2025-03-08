#pragma once
#include "Module.h"
#include <vector>

struct SDL_Window;

namespace module
{
namespace editor
{
class EditorPanel
{
  public:
    virtual void render() const = 0;
    virtual ~EditorPanel() = default;
};
} // namespace editor

class Editor : public module::Module
{
  public:
    bool init() override;

    bool update() override;

    bool shutdown() override;

    template <typename Panel> void addPanel()
    {
        static_assert(std::is_base_of_v<editor::EditorPanel, Panel>, "Panel must inherit from EditorPanel");
        panels.emplace_back(new Panel());
    }

  private:
    std::vector<editor::EditorPanel *> panels;
};
} // namespace module