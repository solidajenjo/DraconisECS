#include "app.h"
#include "modules/window.h"
#include "modules/input.h"
#include "modules/render.h"

app::App* app::app = new app::App();

bool app::init()
{
    app->window = new module::Window();
	app->input = new module::Input();
	app->render = new module::Render();

    return app->window->init("DraconisECS", 800, 600)
		&& app->render->init();
}

bool app::update()
{
	if (app->shouldClose)
	{
		return false;
	}

	app->render->preUpdate();

	app->render->update();
    app->window->update();
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
