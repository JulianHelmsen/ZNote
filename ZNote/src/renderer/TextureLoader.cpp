#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/glew.h>


namespace utils {

	static uint32_t s_whiteTexture = 0xFFFFFFFF;

	uint32_t TextureLoader::GetWhiteTexture() {
		if (s_whiteTexture != 0xFFFFFFFF)
			return s_whiteTexture;



		glGenTextures(1, &s_whiteTexture);
		glBindTexture(GL_TEXTURE_2D, s_whiteTexture);

		uint32_t data = 0xFFFFFFFF;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		return s_whiteTexture;
	}

	uint32_t TextureLoader::LoadTexture(const char* filepath, glm::vec2* size) {
		stbi_set_flip_vertically_on_load(true);
		
		uint32_t texId;
		
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		int textureWidth, textureHeight, components;
		unsigned char* data = stbi_load(filepath, &textureWidth, &textureHeight, &components, 4);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		stbi_image_free(data);

		if (size) {
			size->x = (float) textureWidth;
			size->y = (float) textureHeight;
		}
		return texId;
	}

	uint32_t TextureLoader::LoadTexture(const unsigned char* imageData, uint32_t width, uint32_t height, uint32_t numChannels) {
		uint32_t texId;

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);

	
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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, imageData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		return texId;
	}
}

