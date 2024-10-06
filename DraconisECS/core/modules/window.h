#pragma once

struct SDL_Window;
struct SDL_Renderer;

namespace module
{
    class Window
    {
    public:
 
		bool init(const char* title, int width, int height);

        void clear();

        void present();

		void shutdown();

		SDL_Window* getWindow() const { return window; }
        
    private:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
    };
} // namespace window