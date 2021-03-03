#include "RenderDefaults.h"
#include <GL/glew.h>

namespace renderer {

	void SetRenderDefaults() {
		glLineWidth(3);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}
}