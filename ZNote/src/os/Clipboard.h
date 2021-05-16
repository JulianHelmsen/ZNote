#pragma once

#include <stdint.h>
#include <functional>

namespace os {

	struct ClipboardImage {
		uint32_t width;
		uint32_t height;
		unsigned char* imageData;
		uint32_t numChannels;
	};


	class Clipboard {
	public:

		static void Enumerate();


		template<typename T>
		static void SetDataCallback(std::function<void(const T&)> callback);
	private:

		static void OnImageAvailable();
	

	};
}