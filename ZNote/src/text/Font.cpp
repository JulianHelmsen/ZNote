#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/fterrors.h>
#include "Font.h"
#include <core/Logger.h>
#include <unordered_map>
#include <GL/glew.h>

#define FREETYPE_CALL(x) {FT_Error err = x; if(err) LOG("FreeType error in %s:%d. Reason[\"%s\"]", __FILE__, __LINE__, FT_Error_String(err));}


namespace app {

	static FT_Library lib;

	static bool s_fontsSupported = true;
	static Font* s_font = NULL;



	Font* Font::LoadFont(const char* filepath) {


		FREETYPE_CALL(FT_Init_FreeType(&lib));

		bool useDefaultFont = false;
		if (!filepath) {
			useDefaultFont = true;

#ifdef WINDOWS
			filepath = "C:/Windows/Fonts/arial.ttf";
#else
			ASSERT(false) :
#endif
		}

		FT_Face face;
		FT_Error error = FT_New_Face(lib, filepath, 0, &face);
		if (error) {
			FREETYPE_CALL(error);

			if (useDefaultFont) {
				s_fontsSupported = false;
				LOG("Failed to load default font!\n");
			}
			return NULL;
		}

		FREETYPE_CALL(FT_Set_Pixel_Sizes(face, 0, 48));

		if (s_font) {
			s_font->Delete();
			delete s_font;
		}

		s_font = new Font;

		uint32_t fontWidth = 0;
		uint32_t height = 0;
	
		for (int i = 0; i < 126; i++) {
			Glyph* glyph = s_font->m_glyphs + i;

			if (FT_Load_Char(face, i, FT_LOAD_NO_BITMAP)) {
				LOG("Character %c (dec code: %d) could not be loaded!\n", (char) i, i);
				continue;
			}

			const auto& loadedGlyph = face->glyph;

			// copy glyph data
			glyph->width = loadedGlyph->bitmap.width;
			glyph->height = loadedGlyph->bitmap.rows;
			glyph->bearingX = loadedGlyph->bitmap_left;
			glyph->bearingY = loadedGlyph->bitmap_top;
			glyph->advance = loadedGlyph->advance.x;

			// calculate the space in x dir the character needs
			fontWidth += glyph->width;
			height = loadedGlyph->bitmap.rows > height ? loadedGlyph->bitmap.rows : height;
		}

		ASSERT(height > 0);

		glGenTextures(1, &s_font->m_textureId);
		glBindTexture(GL_TEXTURE_2D, s_font->m_textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fontWidth, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

		uint32_t xOffset = 0;

		for (int i = 0; i < 126; i++) {
			Glyph* glyph = s_font->m_glyphs + i;

			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				continue;
			}

			const auto& bitmap = face->glyph->bitmap;

			// write characters to texture
			if (bitmap.buffer) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, 0, bitmap.width, bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
			}
			
			
			// calculate uv of bottom left corner
			glyph->texStart = glm::vec2((float) xOffset / fontWidth, 0);
			glyph->texSize = glm::vec2((float) bitmap.width / fontWidth);

			xOffset += bitmap.width;
		}
		
		FREETYPE_CALL(FT_Done_Face(face));
		return s_font;
	}

	void Font::CleanUp() {
		if(s_font)
			s_font->Delete();
		FREETYPE_CALL(FT_Done_FreeType(lib));
	}

	void Font::Delete() {
		// delete textures
		glDeleteTextures(1, &m_textureId);
	}

	Font* Font::GetCurrentFont() {
		return s_font;
	}
}