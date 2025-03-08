#pragma once
#include "Module.h"
#include <memory>
#include <vector>

struct SDL_Window;

namespace module
{
namespace editor
{
class EditorPanel
{
  public:
    virtual void render() = 0;
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
        panels.emplace_back(std::make_unique<Panel>());
    }

  private:
    std::vector<std::unique_ptr<editor::EditorPanel>> panels;
};
} // namespace module