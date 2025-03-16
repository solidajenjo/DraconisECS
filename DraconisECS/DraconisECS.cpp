#define SDL_MAIN_HANDLED
#include "app.h"
#include <iostream>

int main()
{
    if (!app::appInstance.init())
    {
        std::cerr << "Failed to initialize app\n";
        return 1;
    }
    LOLOLO
    // Main loop
    while (app::appInstance.update())
    {
    }

    // Shutdown the app
    app::appInstance.shutdown();

    return 0;
}
