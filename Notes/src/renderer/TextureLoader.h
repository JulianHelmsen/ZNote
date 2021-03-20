#pragma once

#include <stdint.h>

namespace utils {

	class TextureLoader {
	public:
		static uint32_t LoadTexture(const char* filepath);
	};

}