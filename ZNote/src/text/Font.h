#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

namespace app {

	class Font {
	public:
		static Font* LoadFont(const char* filepath = NULL);
		static Font* GetCurrentFont();
		static void CleanUp();
		void Delete();

		struct Glyph {
			uint32_t width;
			uint32_t height;
			uint32_t bearingX;
			uint32_t bearingY;
			uint32_t advance;

			glm::vec2 texStart;
			glm::vec2 texSize;
		};

		const Glyph& GetGlyph(char charCode) const { return m_glyphs[charCode]; }
		inline uint32_t GetTextureId() const { return m_textureId; }

	private:
		uint32_t m_textureId;

		Glyph m_glyphs[127];


	};

}