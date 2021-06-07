#include "Tool.h"
#include "core/Application.h"
#include <cassert>

namespace app {
	Tool* Tool::s_usedTool = NULL;


	void Tool::UseTool(Tool* tool) {
		assert(tool);
		if (s_usedTool)
			delete s_usedTool;

		s_usedTool = tool;
		tool->SetContext(&Application::GetActiveScene());
	}
}
