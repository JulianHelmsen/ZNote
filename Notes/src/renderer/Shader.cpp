#include "Shader.h"
#include <GL/glew.h>
#include <string>

namespace utils {

	void CompileShader(uint32_t shader, uint32_t shaderType, const std::string& source) {
		const char* sourceString = source.c_str();
		GLint len = (GLint) source.size();
		glShaderSource(shader, 1, &sourceString, &len);
		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			char infoLog[1020] = { 0 };
			GLsizei len;
			glGetShaderInfoLog(shader, sizeof(infoLog) - 1, &len, infoLog);
			if (shaderType == GL_VERTEX_SHADER)
				puts("Vertex shader:");
			else if (shaderType == GL_FRAGMENT_SHADER)
				puts("Fragment shader:");
			puts(infoLog);
		}
	}

	uint32_t CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) {
		uint32_t programId = glCreateProgram();
		uint32_t vertexId = glCreateShader(GL_VERTEX_SHADER);
		uint32_t fragmentId = glCreateShader(GL_FRAGMENT_SHADER);

		CompileShader(vertexId, GL_VERTEX_SHADER, vertexSource);
		CompileShader(fragmentId, GL_FRAGMENT_SHADER, fragmentSource);

		glAttachShader(programId, vertexId);
		glAttachShader(programId, fragmentId);
		glLinkProgram(programId);

		glValidateProgram(programId);

		GLint status;
		glGetProgramiv(programId, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			char infoLog[1020] = { 0 };
			GLsizei len;
			glGetProgramInfoLog(programId, sizeof(infoLog) - 1, &len, infoLog);
			puts("Linking error:");
			puts(infoLog);
		}

		glDeleteShader(vertexId);
		glDeleteShader(fragmentId);

		return programId;
	}
#if 0
	a = (color & 0xFF;
	b = (color >> 8) & 0xFF;
	g = (color >> 16) & 0xFF;
	r = (color >> 24) & 0xFF;
#endif

	uint32_t CreateLineShaderProgram() {
		std::string vertexSource = ""
			"#version 400 core\n"
			"layout(location = 0) in vec2 position;\n"
			"layout(location = 1) in uint color;\n"
			"uniform mat4 viewProjectionMatrix;"
			"out vec4 inColor;\n"
			"void main() {\n"
			"	gl_Position = viewProjectionMatrix * vec4(position, 0.0, 1.0);\n"
			"	uint b = (color >> 8) & 0xFF;\n"
			"	uint g = (color >> 16) & 0xFF;\n"
			"	uint r = (color >> 24) & 0xFF;\n"
			"	uint a = color & 0xFF;\n"
			"	inColor = vec4(float(r) / 255, float(g) / 255, float(b) / 255, float(a) / 255);\n"
			"}\n";

		std::string fragmentSource =
			"#version 400 core\n"
			"in vec4 inColor;\n"
			"out vec4 outColor;\n"
			"void main() {\n"
			"	outColor = inColor;\n"
			"}\n";
		return CreateShaderProgram(vertexSource, fragmentSource);
	}

	uint32_t CreateImageShaderProgram() {
		std::string vertexSource = ""
			"#version 400 core\n"
			"layout(location = 0) in vec2 position;\n"
			"layout(location = 1) in vec2 uv;\n"
			"layout(location = 2) in uint texId;\n"
			"uniform mat4 viewProjectionMatrix;\n"
			"flat out uint f_texId;\n"
			"out vec2 outUv;\n"
			"void main() {\n"
			"	gl_Position = viewProjectionMatrix * vec4(position, 0.0, 1.0);\n"
			"	outUv = uv;\n"
			"	f_texId = texId;\n"
			"}\n";

		std::string fragmentSource =
			"#version 400 core\n"
			"in vec2 outUv;\n"
			"out vec4 outColor;\n"
			"uniform sampler2D u_textures[32];\n"
			"flat in uint f_texId;\n"
			"void main() {\n"
			"	outColor = texture(u_textures[f_texId], outUv);\n"
			"}\n";
		return CreateShaderProgram(vertexSource, fragmentSource);
	}
	
}