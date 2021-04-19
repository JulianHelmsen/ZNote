#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

namespace utils {

	class TextureLoader {
	public:
		static uint32_t LoadTexture(const char* filepath, glm::vec2* size);
		static uint32_t LoadTexture(const char* filepath) { return LoadTexture(filepath, NULL); }
	};

}