#include "app.h"
#include "modules/window.h"
#include "modules/input.h"
#include "modules/render.h"
#include "modules/editor.h"

app::App< //keep up to date .h declaration
	module::Window, 
	module::Input, 
	module::Render, 
	module::Editor
> app::appInstance;


