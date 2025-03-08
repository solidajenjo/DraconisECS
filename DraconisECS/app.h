#pragma once
#include "Module.h"
#include "core/modules/editor.h"
#include "core/modules/input.h"
#include "core/modules/render.h"
#include "core/modules/window.h"
#include <tuple>

#define MODULES module::Window, module::Input, module::Render, module::Editor

namespace app
{
template <typename... Modules> class App
{
    static_assert((std::is_base_of_v<module::Module, Modules> && ...), "All types must inherit from module::Module");

  private:
    std::tuple<Modules...> modules;
    bool shouldClose = false;

  public:
    // Initialize all modules
    bool init()
    {
        auto res = std::apply([&](auto &...module) { return (... && module.init()); }, modules);
        return res;
    }

    // Update all modules
    bool update()
    {
        if (shouldClose)
            return false;

        auto res = std::apply([&](auto &...module) { return (... && module.preUpdate()); }, modules);
        if (!res)
            return false;
        res = std::apply([&](auto &...module) { return (... && module.update()); }, modules);
        if (!res)
            return false;
        res = std::apply([&](auto &...module) { return (... && module.postUpdate()); }, modules);

        return res;
    }

    // Shutdown all modules
    bool shutdown()
    {
        std::apply([&](auto &...module) { return (... && module.shutdown()); }, modules);
        return true;
    }

    // Quit the app
    void quit()
    {
        shouldClose = true;
    }

    // Get a specific module by type
    template <typename T> T &getModule()
    {
        return std::get<T>(modules);
    }
};

extern App<MODULES> appInstance;
} // namespace app
