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
			int32_t bearingX;
			int32_t bearingY;
			float advance;

			glm::vec2 texStart;
			glm::vec2 texSize;
		};

		const Glyph& GetGlyph(char charCode) const { return m_glyphs[charCode]; }
		inline uint32_t GetTextureId() const { return m_textureId; }

		float GetFontSize() const { return m_fontSize; }

		void SetFontSize(float size) { m_fontSize = size; }

		/*
		* returns the size of the font atlas in pixels. The height of the texture is the height of the tallest character
		*/
		const glm::vec2& GetTextureSize() const { return m_textureSize; }

	private:
		glm::vec2 m_textureSize;


		float m_fontSize = 0.2f;
		uint32_t m_textureId;

		Glyph m_glyphs[127];


	};

}