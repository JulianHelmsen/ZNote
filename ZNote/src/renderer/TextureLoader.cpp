#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/glew.h>


namespace utils {

	static uint32_t s_whiteTexture = 0xFFFFFFFF;


	static app::Image LoadTexture(const void* imageData, uint32_t width, uint32_t height, uint32_t numChannels);

	static void SetTexParameters() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	}

	uint32_t TextureLoader::GetWhiteTexture() {
		if (s_whiteTexture != 0xFFFFFFFF)
			return s_whiteTexture;



		glGenTextures(1, &s_whiteTexture);
		glBindTexture(GL_TEXTURE_2D, s_whiteTexture);

		uint32_t data = 0xFFFFFFFF;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);

		SetTexParameters();
		
		return s_whiteTexture;
	}

	app::Image TextureLoader::LoadTexture(const char* filepath) {
		stbi_set_flip_vertically_on_load(true);
		
		app::Image image;

		
		glGenTextures(1, &image.textureId);
		glBindTexture(GL_TEXTURE_2D, image.textureId);
		int textureWidth, textureHeight, components;
		unsigned char* data = stbi_load(filepath, &textureWidth, &textureHeight, &components, 4);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		SetTexParameters();

		stbi_image_free(data);

		image.textureWidth = (uint32_t) textureWidth;
		image.textureHeight = (uint32_t) textureHeight;
		
		image.size.x = (float) textureWidth;
		image.size.y = (float) textureHeight;

		image.filepath = filepath;
		
		return image;
	}
	void TextureLoader::DeleteTexture(uint32_t texId) {
		glDeleteTextures(1, &texId);
	}
	app::Image TextureLoader::LoadTexture(const void* imageData, uint32_t width, uint32_t height, uint32_t numChannels) {
		app::Image image;

		image.textureWidth = width;
		image.textureHeight = height;
		image.size = glm::vec2((float)width, (float)height);
		image.filepath = "(none)";
		

		glGenTextures(1, &image.textureId);
		glBindTexture(GL_TEXTURE_2D, image.textureId);

	
		GLenum dataFormat;
		switch (numChannels) {
		case 3:
			dataFormat = GL_RGB;
			break;
		case 1:
			dataFormat = GL_RED;
			break;
		case 4:
			dataFormat = GL_RGBA;
			break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, imageData);
		SetTexParameters();

		return image;
	}
}

