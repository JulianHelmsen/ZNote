#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

namespace utils {

	class TextureLoader {
	public:

		static uint32_t GetWhiteTexture();
		static uint32_t LoadTexture(const char* filepath, glm::vec2* size);
		static uint32_t LoadTexture(const char* filepath) { return LoadTexture(filepath, NULL); }
		static uint32_t LoadTexture(const void* imageData, uint32_t width, uint32_t height, uint32_t numChannels);
	};

}