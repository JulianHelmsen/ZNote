#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include "Scene.h"

namespace utils {

	class TextureLoader {
	public:

		static uint32_t GetWhiteTexture();
		static app::Image LoadTexture(const char* filepath);
		static app::Image LoadTexture(const void* imageData, uint32_t width, uint32_t height, uint32_t numChannels);

		static void DeleteTexture(uint32_t texId);
		static void DeleteImage(const app::Image& image) { DeleteTexture(image.textureId); }
	};

}