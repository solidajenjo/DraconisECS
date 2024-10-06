#include "SDL.h"
#include <iostream>

namespace module
{
    class Window
    {
    public:
 
        Window(const char* title, int width, int height)
        {
            window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
            if (window == nullptr)
            {
                std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
                exit(1);
            }
        }

        ~Window()
        {
            SDL_DestroyWindow(window);
        }

        void clear()
        {
            SDL_RenderClear(renderer);
        }

        void present()
        {
            SDL_RenderPresent(renderer);
        }

        void setRenderer(SDL_Renderer* renderer)
        {
            this->renderer = renderer;
        }

        SDL_Window* getWindow()
        {
            return window;
        }

    private:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
    };
} // namespace window