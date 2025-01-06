#define SDL_MAIN_HANDLED
#include "app.h"
#include <iostream>

int main() 
{
    if (!app::appInstance.init()) {
        std::cerr << "Failed to initialize app\n";
        return 1;
    }

    // Main loop
    while (app::appInstance.update()) {
        std::cout << "App running...\n";
    }

    // Shutdown the app
    app::appInstance.shutdown();

    return 0;
}