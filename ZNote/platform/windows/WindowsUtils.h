#pragma once

#include <Windows.h>
#include <string>

#ifdef DEBUG
	#define WIN32_CALL(x) {UINT res = x; if(!res) LOG("win32 api error [%s]: %s\n", #x, ToErrorString(GetLastError()).c_str()); }
#else
	#define WIN32_CALL(x) x
#endif

namespace os {


	std::string ToErrorString(DWORD errorcode) {
		LPSTR messageBuffer;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &messageBuffer, 0, NULL);
		std::string msg(messageBuffer, size);
		LocalFree(messageBuffer);
		return msg;
	}
}