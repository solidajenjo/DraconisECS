#include "app.h"
#include "modules/window.h"
#include "modules/input.h"

app::App* app::app = new app::App();

bool app::init()
{
    app->window = new module::Window();
	app->input = new module::Input();

    return app->window->init("DraconisECS", 800, 600);
}

bool app::update()
{
	if (app->shouldClose)
	{
		return false;
	}

    app->window->clear();
    app->window->present();

	app->input->update();

    return true;
}

bool app::shutdown()
{
	delete app->window;

    return true;
}

void app::quit()
{
	app->shouldClose = true;
}
