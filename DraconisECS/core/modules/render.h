#pragma once
#include "Module.h"

namespace module
{
	class Render : public module::Module //TODO: SHUTDOWN OPENGL
	{
	public:
		bool init();
		bool preUpdate() override;
		bool update() override;
	};
}