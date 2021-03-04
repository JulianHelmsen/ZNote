#include "Pencil.h"
#include "Keycodes.h"

namespace app {

	Pencil::Pencil() : colorPalleteIdx(0), m_newStroke(true) {
		colorPallete[0] = PINK;
		colorPallete[1] = WHITE;
		colorPallete[2] = CORAL;
		colorPallete[3] = YELLOW;
		colorPallete[4] = TEAL;
		colorPallete[5] = LIGHT_BLUE;
		colorPallete[6] = BLUE;
		colorPallete[7] = PURPLE;
	}
	

	void Pencil::OnDrag(const glm::vec2& prev, const glm::vec2& newpos, int button) {
		// check if the last line is connected to this one and wether they can be merged into one line segment
		if (m_newStroke) {
			m_context->hostIndices.push_back((uint32_t) m_context->hostVertices.size());
			m_context->hostVertices.push_back({ prev.x, prev.y, GetColor() });
			m_context->hostIndices.push_back((uint32_t) m_context->hostVertices.size());
			m_context->hostVertices.push_back({ newpos.x, newpos.y, GetColor() });
			m_newStroke = false; // until release
		}else {
			AddLineSegment(newpos);
		}
	}

	void Pencil::OnButtonStateChanged(MouseButton button, bool down) {
		if (button == MouseButton::WHEEL && down) {
			colorPalleteIdx++;
			if (colorPalleteIdx >= sizeof(colorPallete) / sizeof(Color))
				colorPalleteIdx = 0;
		}
		else if (button == MouseButton::LEFT) {
			if (!down)
				m_newStroke = true;
		}
	}

	void Pencil::OnKeyPress(uint32_t keycode) {
		if (keycode >= KEY_1 && keycode <= KEY_8)
			colorPalleteIdx = keycode - KEY_1;
	}

	void Pencil::OnScroll(int dir) {

	}

	void Pencil::AddLineSegment(const glm::vec2& newpos) {
		Vertex& last = m_context->hostVertices[m_context->hostVertices.size() - 1];
		const Vertex& penultimate = m_context->hostVertices[m_context->hostVertices.size() - 2];
		glm::vec2 lastPos = glm::vec2(last.x, last.y);
		glm::vec2 prevDir = lastPos - glm::vec2(penultimate.x, penultimate.y);
		glm::vec2 currDir = newpos - lastPos;

		float d = glm::dot(glm::normalize(prevDir), glm::normalize(currDir));
		float angle = glm::acos(d);

		// calculate nearest position to possibly extended line to calculate distance
		// line: s = lastPos, dir: prevDir
		// p: newpos
		// calculate parameter
		glm::vec2 posDiff = newpos - lastPos;
		const float t = glm::dot(posDiff, prevDir) / (prevDir.x * prevDir.x + prevDir.y * prevDir.y);
		glm::vec2 nearestPointOnExtendedLine = lastPos + t * prevDir;
		float distanceToPoint = glm::length(nearestPointOnExtendedLine - newpos);
		float extendedLineLen = glm::length(nearestPointOnExtendedLine - glm::vec2(penultimate.x, penultimate.y));
		float oldLineLen = glm::length(lastPos - glm::vec2(penultimate.x, penultimate.y));

		float distanceRatio = distanceToPoint / extendedLineLen;

		constexpr float distanceRatioThreshold = 0.02f;

		if (distanceRatio < distanceRatioThreshold && t > 0.0f && oldLineLen < extendedLineLen) {
			last.x = newpos.x;
			last.y = newpos.y;
		}else {
			m_context->hostIndices.push_back((uint32_t)m_context->hostVertices.size() - 1);
			m_context->hostIndices.push_back((uint32_t)m_context->hostVertices.size());
			m_context->hostVertices.push_back({ newpos.x, newpos.y, GetColor() });
		}
	}
}