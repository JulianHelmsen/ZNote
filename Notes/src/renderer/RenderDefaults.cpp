#include "RenderDefaults.h"
#include <GL/glew.h>
#ifdef DEBUG
#include <stdio.h>
#endif

namespace renderer {

#ifdef DEBUG

	void GLAPIENTRY
		MessageCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
	{
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
	}

	void EnableDebugCallback() {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
	}
#endif

	void SetRenderDefaults() {
#ifdef DEBUG
		EnableDebugCallback();
#endif
		glLineWidth(3);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}
}