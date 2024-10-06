#pragma once

namespace module
{
	class Render
	{
	public:
		bool init();
		void preUpdate();
		void update();
		void postUpdate();
		void shutdown();
	};
}