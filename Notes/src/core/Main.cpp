#include "Application.h"

int main(const int argc, const char** argv) {
	app::Application app;
	app.Run();
	app.OnClose();

	return 0;
}