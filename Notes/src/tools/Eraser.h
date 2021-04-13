#pragma once

#include "Tool.h"

namespace app {

	class Eraser : public Tool {
	public:
		Eraser() : m_size(0.02f) {}
		void OnDrag(const glm::vec2& prev, const glm::vec2& newpos, int button) override;
		void OnButtonStateChanged(MouseButton button, bool down) override;
		void OnKeyPress(uint32_t keycode) override;
		void OnScroll(int dir) override;
		

	private:
		void RemoveLine(uint32_t idxBufferOffset1, uint32_t idxBufferOffset);
		static uint32_t GetVertexUsageCount(const Scene* context, uint32_t indexBufferOffset);
		static bool CalculateIntersection(const glm::vec2& pos1, const glm::vec2& dir1, const glm::vec2& pos2, const glm::vec2 dir2, float* t, float* r);
		float m_size;
	};

}