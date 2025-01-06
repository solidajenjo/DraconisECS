#include "input.h"
#include "app.h"
#include "SDL.h"
#include "backends/imgui_impl_sdl2.h"
#include "modules/window.h"

bool module::Input::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			app::appInstance.quit();
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(app::appInstance.getModule<module::Window>().getWindow()))
			app::appInstance.quit();
	}
	return true;
}
