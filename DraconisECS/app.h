#pragma once
#include <tuple>
#include "Module.h"
#include "modules/Window.h"
#include "modules/Input.h"
#include "modules/Render.h"
#include "modules/Editor.h"

namespace app
{
	template <typename... Modules>
	struct App
	{
    private:
        std::tuple<Modules...> modules;
        bool shouldClose = false;

        // Helper function to iterate over tuple
        template <typename Func, typename Tuple, std::size_t... Indices>
        void forEachImpl(Func&& func, Tuple&& tuple, std::index_sequence<Indices...>) {
            (func(std::get<Indices>(tuple)), ...);
        }

        template <typename Func, typename Tuple>
        void forEach(Func&& func, Tuple&& tuple) {
            forEachImpl(std::forward<Func>(func), tuple,
                std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
        }

    public:
        // Initialize all modules
        bool init() 
        {
#pragma warning(push)
#pragma warning(disable: 4715)
            forEach([&](auto& module) 
                {
                    if (!module.init()) 
                    {
                        return false;
                    }
                }, modules);
            return true;
#pragma warning(pop)
        }

        // Update all modules
        bool update() 
        {
#pragma warning(push)
#pragma warning(disable: 4715)
            if (shouldClose) return false;
			forEach([&](auto& module)
				{
					if (!module.preUpdate())
					{
						return false;
					}
				}, modules);

            forEach([&](auto& module) 
                {
                    if (!module.update()) {
                        return false;
                    }
                }, modules);

			forEach([&](auto& module)
				{
					if (!module.postUpdate()) {
						return false;
					}
				}, modules);
            return true;
#pragma warning(pop)
        }

        // Shutdown all modules
        bool shutdown() 
        {
#pragma warning(push)
#pragma warning(disable: 4715)
            forEach([](auto& module) 
                {
                    module.shutdown();
                }, modules);
            return true;
#pragma warning(pop)
        }

        // Quit the app
        void quit() 
        {
            shouldClose = true;
        }

        // Get a specific module by type
        template <typename T>
        T& getModule() 
        {
            return std::get<T>(modules);
        }
	};

    extern App< //Keep up to data cpp defintion
        module::Window, 
        module::Input, 
        module::Render, 
        module::Editor
    > appInstance;
} // namespace app
