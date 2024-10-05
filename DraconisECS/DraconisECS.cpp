#define SDL_MAIN_HANDLED
#include <iostream>
#include "SDL.h"
#include "ecs.h"
#include "modules/window.h"

// Example component classes
struct PositionComponent {
};
struct VelocityComponent {
};
struct HealthComponent {
};

int main() {
    //SDL_SetMainReady();

    ecs::ChunkManager manager;

    manager.createEntity(PositionComponent{}, VelocityComponent{});
    manager.createEntity(PositionComponent{}, HealthComponent{});
    manager.createEntity(VelocityComponent{}, HealthComponent{});

    std::cout << "----------------" << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    module::Window window("ECS", 800, 600);

    while(1)
    {
        window.clear();
        window.present();
    }
   
    //clear sdl
	SDL_Quit();

    return 0;
}