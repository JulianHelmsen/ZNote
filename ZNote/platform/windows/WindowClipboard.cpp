#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "os/Clipboard.h"
#include <Windows.h>
#include <core/Logger.h>
#include "WindowsUtils.h"
#include "core/Window.h"


namespace os {

	static std::function<void(const ClipboardImage&)> s_Imagecallback = NULL;

	void Clipboard::OnImageAvailable() {
		HBITMAP handle = (HBITMAP) GetClipboardData(CF_BITMAP);
		if (handle == NULL) {
			LOG("GetClipboardData: %s\n", ToErrorString(GetLastError()).c_str());
		}

		BITMAPINFO info{ 0 };
		BITMAP bitmap = { 0 };

		HDC dc = CreateCompatibleDC(NULL);
		if(dc == NULL)
			LOG("CreateCompatibleDC: %s\n", ToErrorString(GetLastError()).c_str());

		SelectObject(dc, handle);
		GetObject(handle, sizeof(bitmap), &bitmap);


		BITMAPINFOHEADER& header = info.bmiHeader;
		header.biSize = sizeof(BITMAPINFOHEADER);
		header.biWidth = bitmap.bmWidth;
		header.biHeight = bitmap.bmHeight;
		header.biPlanes = 1;
		header.biBitCount = 32;
		header.biCompression = BI_RGB;
	

		
		unsigned char* imageData = new unsigned char[bitmap.bmWidth * bitmap.bmHeight * 4];
		int val = GetDIBits(dc, handle, 0, bitmap.bmHeight, imageData, &info, DIB_RGB_COLORS);
		if (val == 0) {
			LOG("Retrieving pixels failed\n");
		}
		DeleteDC(dc);


		// Swizzle color components
		// This is done in a bad way. Refactor later
		uint32_t imageSize = bitmap.bmWidth * bitmap.bmHeight;
		uint32_t offset = 0;
		for (uint32_t i = 0; i < imageSize; i++) {
			unsigned char tmp = imageData[offset + 0];
			imageData[offset + 0] = imageData[offset + 2];
			imageData[offset + 2] = tmp;
			offset += 4;
		}

		ClipboardImage image;
		image.imageData = imageData;
		image.numChannels = 4;
		image.width = (uint32_t)bitmap.bmWidth;
		image.height = (uint32_t)bitmap.bmHeight;
		s_Imagecallback(image);
		delete[] imageData;
	}

	void Clipboard::Enumerate() {
		HWND hwnd = glfwGetWin32Window((GLFWwindow*)Window::GetNativeHandle());
		BOOL openSuccess = OpenClipboard(hwnd);

		if(!openSuccess)
			LOG("open clipboard failed: %s\n", ToErrorString(GetLastError()).c_str());

		UINT format = 0;
		do{
			format = EnumClipboardFormats(format);

			if (format == CF_BITMAP && s_Imagecallback) {
				// clipboard image available
				OnImageAvailable();
				break;
			}

		} while (format != 0);

		UINT error = GetLastError();
		if (error != ERROR_SUCCESS) {
			LOG("EnumClipboardFormats: %s\n", ToErrorString(error).c_str());
		}

		BOOL closeSuccess = CloseClipboard();
		if (!closeSuccess)
			LOG("close clipboard failed: %s\n", ToErrorString(GetLastError()).c_str());
		
		

	}

	template<>
	void Clipboard::SetDataCallback<ClipboardImage>(std::function<void(const ClipboardImage&)> callback) {
		s_Imagecallback = callback;
	}

}
