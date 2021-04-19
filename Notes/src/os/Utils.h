#pragma once

#include <optional>
#include <string>

#ifdef NDEBUG
	#define RELEASE_CLOSE_CONSOLE() os::HideConsoleWindow()
#else
	#define RELEASE_CLOSE_CONSOLE()
#endif

namespace os {

	std::optional<std::string> ShowOpenDialog(const char* fileformat);
	std::optional<std::string> ShowSaveDialog();

	void HideConsoleWindow();

	
}