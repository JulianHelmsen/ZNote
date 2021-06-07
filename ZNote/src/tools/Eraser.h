#pragma once

#include "Tool.h"

namespace app {

	class Eraser : public Tool {
	public:
		Eraser() : m_size(0.02f) {}
		void OnDrag(MouseButton button, const glm::vec2& prev, const glm::vec2& newpos) override;
		

	private:
		void RemoveLine(uint32_t idxBufferOffset1, uint32_t idxBufferOffset);
		static uint32_t GetVertexUsageCount(const Scene* context, uint32_t indexBufferOffset);
		static bool CalculateIntersection(const glm::vec2& pos1, const glm::vec2& dir1, const glm::vec2& pos2, const glm::vec2 dir2, float* t, float* r);
		float m_size;
	};


}