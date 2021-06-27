#include "Application.h"
#include "os/Utils.h"

int main(const int argc, const char** argv) {
#ifndef DEBUG
	os::HideConsoleWindow();
#endif
	app::Application app;
	app.Run();
	app.OnClose();

#ifndef DEBUG
	os::ShowConsoleWindow();
#endif
	return 0;
}