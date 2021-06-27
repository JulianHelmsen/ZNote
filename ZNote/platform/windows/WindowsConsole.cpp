#include "os/Utils.h"
#include <Windows.h>

namespace os {


	void HideConsoleWindow() {
		HWND consoleWindow = GetConsoleWindow();
		BOOL hidden = ShowWindow(consoleWindow, SW_HIDE);
	}


	void ShowConsoleWindow() {
		HWND consoleWindow = GetConsoleWindow();
		BOOL shown = ShowWindow(consoleWindow, SW_SHOW);
	}
}