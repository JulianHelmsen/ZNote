#pragma once

#define WHITE (0xFFFFFFFF)
#define PINK (0xFB73BEFF)
#define CORAL (0xFF8D71FF)
#define YELLOW (0xFFE066FF)
#define TEAL (0x59F3CEFF)
#define LIGHT_BLUE (0x65D0FAFF)
#define BLUE (0x4984F2FF)
#define PURPLE (0xAB7DFFFF)

namespace app {
	union Color{
		Color(uint32_t code) : color(code) {}
		Color(uint32_t r, uint32_t g, uint32_t b) : Color(r, g, b, 255) {}
		Color(uint32_t r, uint32_t g, uint32_t b, uint32_t a) : r((uint8_t)r), g((uint8_t)g), b((uint8_t)b), a((uint8_t)a) {}
		Color() : color(0xFFFFFFFF) {}

		struct {
			uint8_t a;
			uint8_t b;
			uint8_t g;
			uint8_t r;
		};
		uint32_t color;
	};

}