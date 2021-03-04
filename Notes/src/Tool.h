#pragma once

#include <glm/glm.hpp>
#include "core/Window.h"
#include "Scene.h"

namespace app {

	class Tool {
	public:
		virtual void OnDrag(const glm::vec2& prev, const glm::vec2& newpos, int button) = 0;
		virtual void OnButtonStateChanged(MouseButton button, bool down) = 0;
		virtual void OnKeyPress(uint32_t keycode) = 0;
		virtual void OnScroll(int dir) = 0;

		inline void SetContext(Scene* context) { m_context = context; }
	protected:
		Scene* m_context;
	};
}