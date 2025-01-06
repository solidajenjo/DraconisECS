#pragma once
#include "Module.h"

struct SDL_Window;
struct SDL_Renderer;

namespace module
{
	class Window : public module::Module
    {
    public:
 
		bool init() override;

        bool postUpdate() override;

		bool shutdown() override;

		SDL_Window* getWindow() const { return window; }
        
    private:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
    };
} // namespace window