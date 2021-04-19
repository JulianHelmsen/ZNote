#include "Application.h"
#include "os/Utils.h"

int main(const int argc, const char** argv) {
	RELEASE_HIDE_CONSOLE();
	app::Application app;
	app.Run();
	app.OnClose();

	return 0;
}