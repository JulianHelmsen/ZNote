#include "TransformTool.h"
#include "core/Logger.h"
#include "core/Application.h"
#include "renderer/Renderer2D.h"

#define AXIS_NONE (0)
#define AXIS_X_BIT (1)
#define AXIS_Y_BIT (2)

namespace app {

	static glm::vec2 s_defaultArrowSize = glm::vec2(0.2f, 0.2f);


	uint32_t TransformTool::GetAxis(const glm::vec2& pos) {
		glm::vec2 guizmoPos = GetGuizmoPosition();
		glm::vec2 relativePos = pos - guizmoPos;
		
		glm::vec2 axisMargin = glm::vec2(0.05f, 0.05f);
		
		if (!AABB(pos, guizmoPos - axisMargin, m_arrowSize + axisMargin)) {
			// the given position is not inside the bounding box of the guizmo
			return AXIS_NONE;
		}

		if (glm::length(relativePos) < 0.1f * glm::length(s_defaultArrowSize)) {
			// select both axis cause the mouse is near the center of the guizmo
			return AXIS_X_BIT | AXIS_Y_BIT;
		}

		if (relativePos.x < s_defaultArrowSize.x * 0.1f) {
			// mouse close to y-axis cause the projection of the position has a small value when projected on the x-axis
			return AXIS_Y_BIT;
		}else if(relativePos.y < s_defaultArrowSize.y * 0.1f) {
			// mouse close to x-axis cause the projection of the position has a small value when projected on the y-axis
			return AXIS_X_BIT;
		}
		return AXIS_NONE;
	}


	void TransformTool::OnButtonStateChanged(MouseButton button, const glm::vec2& mouseWorldPos, bool down) {
		if (HasGuizmo() && down) {
			// check if axis was clicked (used)
			uint32_t axis = GetAxis(WorldToGuizmoSpace(mouseWorldPos));
			LOG("%d\n", axis);
			if (axis) {
				// mark this axis as being used
				m_selectedAxis = axis;
				return; // don't select a new target
			}
		}
		
		if (down)
			SelectTarget(mouseWorldPos);
		
		m_selectedAxis = 0; // mouse released or selected new target results in the axis being unused
	}




	void TransformTool::Draw() const {
		glm::vec2 pos = GetGuizmoPosition();

		renderer::Batch<Vertex> batch;

		// construct mesh

		batch.InsertVertex(Vertex{ pos, Color{255, 0, 0} });
		batch.InsertVertex(Vertex{ pos + glm::vec2(m_arrowSize.x, 0.0f), Color{255, 0, 0} });
		batch.InsertVertex(Vertex{ pos, Color{0, 255, 0} });
		batch.InsertVertex(Vertex{ pos + glm::vec2(0.0f, m_arrowSize.y), Color{0, 255, 0} });

		batch.InsertIndex(0);
		batch.InsertIndex(1);
		batch.InsertIndex(2);
		batch.InsertIndex(3);

		Renderer2D::Begin(Application::GetProjectionMatrix());
		Renderer2D::DrawBatch(batch);
		Renderer2D::End();

	}


	bool TransformTool::SelectTarget(const glm::vec2& worldPosition) {
		void* newTarget = NULL;
		TargetType targetType = TargetType::IMAGE;

		for (const Image& image : m_context->images) {
			if (AABB(worldPosition, image.centerPos - image.size * 0.5f, image.size)) {
				newTarget = (void*) &image;
				targetType = TargetType::IMAGE;
			}
		}
		
		if (newTarget != m_target) {
			m_target = newTarget;
			OnTargetSelected(targetType, newTarget);
			return true;
		}
		return false;
	}

	bool TransformTool::HasGuizmo() const {
		return m_target != NULL;
	}	

	void TransformTool::OnTargetSelected(TargetType type, void* target) {
		m_drawIcon = target != NULL;
		m_arrowSize = s_defaultArrowSize;
	}

	glm::vec2 TransformTool::GetTargetPosition() const {
		ASSERT(m_target);
		switch (m_targetType) {
			case TargetType::IMAGE: {
				Image* image = (Image*)m_target;
				return image->centerPos;
			}
		}
		return glm::vec2(0.0f, 0.0f);
	}

	glm::vec2 TransformTool::WorldToGuizmoSpace(const glm::vec2& pos) const {
		const glm::mat4& viewMatrix = Application::GetViewMatrix();
		return viewMatrix * glm::vec4(pos, 1.0f, 1.0f);
	}

	glm::vec2 TransformTool::GuizmoToWorldSpace(const glm::vec2& pos) const {
		glm::mat4 inverseViewMatrix = glm::inverse(Application::GetViewMatrix());
		return inverseViewMatrix * glm::vec4(pos, 1.0f, 1.0f);
	}

	glm::vec2 TransformTool::GetGuizmoPosition() const {
		return WorldToGuizmoSpace(GetTargetPosition());
	}

	bool TransformTool::AABB(const glm::vec2& pos, const glm::vec2& bottomLeft, const glm::vec2 size) {
		bool b1 = pos.x >= bottomLeft.x;
		bool b2 = pos.x <= bottomLeft.x + size.x;
		bool b3 = pos.y >= bottomLeft.y;
		bool b4 = pos.y <= bottomLeft.y + size.y;
		return b1 && b2 && b3 && b4;
	}
}