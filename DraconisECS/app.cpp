#include "app.h"
#include "SDL.h"
#include "modules/window.h"

app::App* app::app = new app::App();

bool app::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    app->window = new module::Window("DraconisECS", 800, 600);

    return true;
}

bool app::update()
{
    app->window->clear();
    app->window->present();
    return true;
}

bool app::shutdown()
{
	delete app->window;

    SDL_Quit();

    return true;
}
