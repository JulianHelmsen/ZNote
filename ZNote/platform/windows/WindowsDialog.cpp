#include "core/Window.h"
#include <Windows.h>
#include "os/Utils.h"
#include "core/Window.h"



namespace os {

	std::optional<std::string> ShowOpenDialog(const char* filter) {
		HWND hwnd = (HWND) Window::GetNativeHandle();

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		return std::nullopt;
	}

	std::optional<std::string> ShowSaveDialog() {
		HWND hwnd = (HWND)Window::GetNativeHandle();

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = NULL;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		return std::nullopt;
	}

	static void ToWideString(wchar_t* dest, const char* src, const uint32_t len) {
		for (uint32_t i = 0; i < len; i++)
			dest[i] = (wchar_t) src[i];
	}

	bool ShowConfirmDialog(const char* title, const char* message) {
		HWND hwnd = (HWND)Window::GetNativeHandle();
		
		wchar_t questionString[1020];
		wchar_t titleString[100];
		uint32_t titleLen = (uint32_t)strlen(title) + 1;
		uint32_t questionLen = (uint32_t)strlen(message) + 1;

		ASSERT(titleLen * 2 < sizeof(titleString));
		ASSERT(questionLen * 2 < sizeof(questionString));

		// convert const char* to wchar_t*
		ToWideString(questionString, message, questionLen);
		ToWideString(titleString, title, titleLen);

		return IDYES == MessageBox(hwnd, questionString, titleString, MB_ICONEXCLAMATION | MB_YESNO);
	}

}