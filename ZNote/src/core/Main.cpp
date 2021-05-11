#include "Application.h"
#include "os/Utils.h"

int main(const int argc, const char** argv) {
	os::HideConsoleWindow();
	app::Application app;
	app.Run();
	app.OnClose();

	return 0;
}