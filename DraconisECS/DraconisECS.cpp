#define SDL_MAIN_HANDLED
#include "app.h"

int main() 
{
	if (!app::init())
	{
		return 1;
	}
	
	while (app::update()){}
	
	app::shutdown();

    return 0;
}