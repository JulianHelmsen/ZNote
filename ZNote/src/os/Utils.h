#pragma once

#include <optional>
#include <string>


namespace os {

	std::optional<std::string> ShowOpenDialog(const char* fileformat);
	std::optional<std::string> ShowSaveDialog();

	void HideConsoleWindow();
	void ShowConsoleWindow();

	bool ShowConfirmDialog(const char* title, const char* message);


	
}