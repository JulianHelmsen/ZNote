#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/glew.h>


namespace utils {

	uint32_t TextureLoader::LoadTexture(const char* filepath) {
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
		return texId;
	}
}

