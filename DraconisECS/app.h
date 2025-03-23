#pragma once
#include <tuple>
#include "Module.h"
#include "core/ecs/ecs.h"
#include "core/modules/editor.h"
#include "core/modules/input.h"
#include "core/modules/render.h"
#include "core/modules/window.h"

#define MODULES module::Window, module::Input, module::Render, module::editor::Editor

namespace app
{
template <typename... Modules>
class App
{
	static_assert( ( std::is_base_of_v<module::Module, Modules> && ... ),
				   "All types must inherit from module::Module" );

private:
	std::tuple<Modules...> modules;
	bool shouldClose = false;

public:
	// Initialize all modules
	bool init()
	{
		auto res = std::apply( [&]( auto &...module ) { return ( ... && module.init() ); }, modules );
		return res;
	}

	// Update all modules
	bool update()
	{
		auto res = std::apply( [&]( auto &...module ) { return ( ... && module.update() ); }, modules );
		return res;
	}

	// Shutdown all modules
	bool shutdown()
	{
		auto res = std::apply( [&]( auto &...module ) { return ( ... && module.shutdown() ); }, modules );
		return res;
	}

	void quit()
	{
		shouldClose = true;
	}

	bool shouldQuit() const
	{
		return shouldClose;
	}

	// Get a specific module by type
	template <typename T>
	T &getModule()
	{
		return std::get<T>( modules );
	}
};

extern App<MODULES> appInstance;
}  // namespace app
