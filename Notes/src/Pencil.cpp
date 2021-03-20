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
			renderer::Batch<Vertex>& batch = m_context->lineBatch;
			renderer::SubMesh<Vertex> mesh;

			mesh.Vertex({ prev, GetColor() });
			mesh.Vertex({ newpos, GetColor() });
			mesh.Index(0);
			mesh.Index(1);

			batch.Insert(mesh);
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
		renderer::Batch<Vertex>& batch = m_context->lineBatch;
		std::vector<Vertex>& vertices = batch.GetVertexList();
		Vertex& last = vertices[vertices.size() - 1];
		const Vertex& penultimate = vertices[vertices.size() - 2];
		
		glm::vec2 prevDir = last.position - penultimate.position;
		glm::vec2 currDir = newpos - last.position;

		float d = glm::dot(glm::normalize(prevDir), glm::normalize(currDir));
		float angle = glm::acos(d);

		// calculate nearest position to possibly extended line to calculate distance
		// line: s = lastPos, dir: prevDir
		// p: newpos
		// calculate parameter
		glm::vec2 posDiff = newpos - last.position;
		const float t = glm::dot(posDiff, prevDir) / (prevDir.x * prevDir.x + prevDir.y * prevDir.y);
		glm::vec2 nearestPointOnExtendedLine = last.position + t * prevDir;
		float distanceToPoint = glm::length(nearestPointOnExtendedLine - newpos);
		float extendedLineLen = glm::length(nearestPointOnExtendedLine - penultimate.position);
		float oldLineLen = glm::length(last.position - penultimate.position);

		float distanceRatio = distanceToPoint / extendedLineLen;

		constexpr float distanceRatioThreshold = 0.02f;

		if (distanceRatio < distanceRatioThreshold && t > 0.0f && oldLineLen < extendedLineLen) {
			last.position = newpos;
		}else {
			batch.InsertIndex(batch.GetNumVertices() - 1);
			batch.InsertIndex(batch.GetNumVertices());
			batch.InsertVertex({ newpos, GetColor() });
		}
	}
}