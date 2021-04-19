#pragma once

#include <optional>
#include <string>

#ifdef NDEBUG
	#define RELEASE_HIDE_CONSOLE() os::HideConsoleWindow()
#else
	#define RELEASE_HIDE_CONSOLE()
#endif

namespace os {

	std::optional<std::string> ShowOpenDialog(const char* fileformat);
	std::optional<std::string> ShowSaveDialog();

	void HideConsoleWindow();

	
}