#include "Eraser.h"
#include <stdio.h>
#include <math.h>

/*

g: x = s + t * u
h: x = p + r * v
*/
namespace app {

	/**
	 * @brief 
	 * 
	 * @param s the first position of the first line
	 * @param u a vector which points from s to the other end of the line. Magnitude = distance
	 * @param p the first position of the second line
	 * @param v a vector which points from p to the other end of the second line. Magnitude = distance
	 * @param t the parameter of the first line which points to the intersection. 
	 * @param r the parameter of the second line which points to the intersection
	 */
	bool Eraser::CalculateIntersection(const glm::vec2& s, const glm::vec2& u, const glm::vec2& p, const glm::vec2 v, float* t, float* r) {
		const float dx = s.x - p.x;
		float vx = v.x;
		if (vx == 0.0f) {
			if (u.x != 0.0f)
				return CalculateIntersection(p, v, s, u, r, t);
			else
				return false;
		}
			

		*t = ((s.y - p.y) - v.y / vx * dx) / (v.y / v.x * u.x - u.y);
		*r = (s.x - p.x + *t * u.x) / vx;
		return true;
	}

	uint32_t Eraser::GetVertexUsageCount(const Scene* context, uint32_t indexBufferOffset) {
		uint32_t vertexIdx = context->hostIndices[indexBufferOffset];
		uint32_t usageCount = 1;

		if (indexBufferOffset > 0 && context->hostIndices[indexBufferOffset - 1] == vertexIdx)
			usageCount++;

		if ((indexBufferOffset + 1) < (uint32_t) context->hostIndices.size() && context->hostIndices[indexBufferOffset + 1] == vertexIdx)
			usageCount++;

		return usageCount;
	}
	void Eraser::RemoveLine(uint32_t idxBufferOffset1, uint32_t idxBufferOffset2) {
		std::vector<uint32_t>& indices = m_context->hostIndices;
		std::vector<Vertex>& vertices = m_context->hostVertices;
		uint32_t firstIdx = indices[idxBufferOffset1];
		uint32_t secondIdx = indices[idxBufferOffset2];
		// Remove indices and possibly vertices

		// asumptions about the index buffer:
		// 1. indices are always sorted
		// 2. indices never connect the same vertex
		// 2. vertex can only be used twice
		//		-> if a index is removed which has no "neighbour" with same value the vertex is no longer used

		bool eraseFirstVertex = true;
		bool eraseSecondVertex = true;
		if (idxBufferOffset1 > 0 && indices[idxBufferOffset1 - 1] == firstIdx) {
			eraseFirstVertex = false;
		}
		if (idxBufferOffset2 + 1 < indices.size() && indices[idxBufferOffset2 + 1] == secondIdx) {
			eraseSecondVertex = false;
		}

		indices.erase(indices.begin() + idxBufferOffset2);
		indices.erase(indices.begin() + idxBufferOffset1);

		// if vertex removed update index buffer
		uint32_t numErased = 0;
		if (eraseSecondVertex) {
			vertices.erase(vertices.begin() + secondIdx);
			numErased++;
		}
		if (eraseFirstVertex) {
			vertices.erase(vertices.begin() + firstIdx);
			numErased++;
		}

		// update all indices
		for (int j = idxBufferOffset1; j < indices.size(); j++) {
			indices[j] -= numErased;
		}
	}

	void Eraser::OnDrag(const glm::vec2& prevMousePos, const glm::vec2& newpos, int button) {
		std::vector<uint32_t>& indices = m_context->hostIndices;
		std::vector<Vertex>& vertices = m_context->hostVertices;

		glm::vec2 mouseDir = newpos - prevMousePos;
		float radius = m_size / m_context->scaleMatrix[0][0];

		for(int i = (int) indices.size() - 1; i >= 1; i -= 2) {
			uint32_t firstIdxOffset = i - 1;
			uint32_t secondIdxOffset  = i;
			uint32_t firstIdx = indices[firstIdxOffset];
			uint32_t secondIdx = indices[secondIdxOffset];

			Vertex& first = vertices[firstIdx];
			Vertex& second = vertices[secondIdx];
			
			glm::vec2 dir = second.position - first.position;



			float t;
			float r;
			CalculateIntersection(prevMousePos, mouseDir, first.position, dir, &t, &r);
			bool intersects = t >= 0.0f && t <= 1.0f && r >= 0.0f && r <= 1.0f;

			bool removeByDistance = glm::length(first.position - newpos) < radius || glm::length(second.position - newpos) < radius;
			
			if (intersects || removeByDistance)
				RemoveLine(firstIdxOffset, secondIdxOffset);
		}
	}

	void Eraser::OnButtonStateChanged(MouseButton button, bool down) {

	}

	void Eraser::OnKeyPress(uint32_t keycode) {

	}

	void Eraser::OnScroll(int dir) {

	}
}