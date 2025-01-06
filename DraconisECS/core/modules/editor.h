#pragma once
#include "Module.h"

struct SDL_Window;

namespace module
{
	class Editor : public module::Module
    {
    public:
 
		bool init() override;

        bool update() override;

    };
} // namespace window