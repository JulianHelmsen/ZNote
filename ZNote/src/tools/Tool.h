#pragma once

#include <glm/glm.hpp>
#include "core/Window.h"
#include "Scene.h"
#include "Keycodes.h"

namespace app {

	class Tool {
	public:
		virtual void OnMouseMove(const glm::vec2& mouse_position) {}
		virtual void OnDrag(MouseButton button, const glm::vec2& prev, const glm::vec2& newpos) {}
		virtual void OnButtonStateChanged(MouseButton button, const glm::vec2& pos, bool down) {}
		virtual void OnKeyPress(uint32_t keycode) {}
		virtual void OnScroll(int dir) {}

		inline void SetContext(Scene* context) { m_context = context; }
		virtual void Draw() const {}

		inline bool WantsToBeDrawn() const { return m_drawIcon; }

		static void UseTool(Tool* tool);
		static Tool* ActiveTool() { return s_usedTool; }
	protected:
		static Tool* s_usedTool;

		Scene* m_context;
		bool m_drawIcon = false;
	};
}