#pragma once

namespace module
{
	class Window;
}

namespace app
{
	class App
	{
	public:
		App() = default;  
		module::Window* window;
	};

	bool init();
	bool update();
	bool shutdown();

	extern App* app;
}
