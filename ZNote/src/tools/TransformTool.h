#pragma once

#include "Tool.h"
#include <glm/glm.hpp>
#include <renderer/Batch.h>

namespace app {




	class TransformTool : public Tool {
	public:
		void OnMouseMove(const glm::vec2& mouse_position) override;
		void OnButtonStateChanged(MouseButton button, const glm::vec2& pos, bool down) override;
		void OnDrag(MouseButton button, const glm::vec2& prev, const glm::vec2& newpos) override;
		void OnKeyPress(uint32_t keycode) override;

		void Draw() const override;
	private:
		enum class TargetType {IMAGE};
		enum class TransformationType {SCALE, TRANSLATION};
		void* m_target = NULL;
		TargetType m_targetType;

		static TransformationType s_transformationType;

		glm::vec2 m_arrowSize;
		uint32_t m_selectedAxis = 0;
		uint32_t m_hoveredAxis = 0;


		void TransformTarget(TransformationType type, const glm::vec2& values);

		bool SelectTarget(const glm::vec2& worldPosition);
		bool HasGuizmo() const;
		glm::vec2 GetGuizmoPosition() const;
		glm::vec2 GetTargetPosition() const;
		glm::vec2 WorldToGuizmoSpace(const glm::vec2& pos) const;
		glm::vec2 GuizmoToWorldSpace(const glm::vec2& pos) const;


		void DrawEnd(renderer::Batch<Vertex>& batch, Color color, const glm::vec2& pos, bool rightDir) const;


		uint32_t GetAxis(const glm::vec2& guizmoSpacePos);
		void OnTargetSelected(TargetType targetType, void* target);
		static bool AABB(const glm::vec2& pos, const glm::vec2& bottomLeft, const glm::vec2 size);
	};
}