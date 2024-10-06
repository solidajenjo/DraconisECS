#pragma once

namespace module
{
	class Window;
	class Input;
}

namespace app
{
	struct App
	{
		App() = default;

		module::Window* window = nullptr;
		module::Input* input = nullptr;

		bool shouldClose = false;
	};

	bool init();
	bool update();
	bool shutdown();

	void quit();

	extern App* app;
}
