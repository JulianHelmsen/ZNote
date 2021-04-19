#pragma once

#include "Tool.h"
#include <glm/glm.hpp>

namespace app {




	class TransformTool : public Tool {
	public:
		void OnButtonStateChanged(MouseButton button, const glm::vec2& pos, bool down) override;
		void OnDrag(const glm::vec2& prev, const glm::vec2& newpos, int button) override;

		void Draw() const override;
	private:
		enum class TargetType {IMAGE};
		enum class TransformationType {SCALE, TRANSLATION};
		void* m_target = NULL;
		TargetType m_targetType;
		TransformationType m_transformationType = TransformationType::TRANSLATION;

		glm::vec2 m_arrowSize;
		uint32_t m_selectedAxis;


		void TransformTarget(TransformationType type, glm::vec2 values);

		bool SelectTarget(const glm::vec2& worldPosition);
		bool HasGuizmo() const;
		glm::vec2 GetGuizmoPosition() const;
		glm::vec2 GetTargetPosition() const;
		glm::vec2 WorldToGuizmoSpace(const glm::vec2& pos) const;
		glm::vec2 GuizmoToWorldSpace(const glm::vec2& pos) const;


		uint32_t GetAxis(const glm::vec2& guizmoSpacePos);
		void OnTargetSelected(TargetType targetType, void* target);
		static bool AABB(const glm::vec2& pos, const glm::vec2& bottomLeft, const glm::vec2 size);
	};
}