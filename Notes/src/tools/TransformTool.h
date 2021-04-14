#pragma once

#include "Tool.h"
#include <glm/glm.hpp>

namespace app {


	class TransformTool : public Tool {
	public:
		void OnButtonStateChanged(MouseButton button, const glm::vec2& pos, bool down) override;

		void SelectImage(Image& image);
		void Draw() const override;
	private:
		enum class TargetType {IMAGE};
		void* m_target;
		TargetType m_targetType;


		template<TargetType T>
		void GetTransform(glm::mat4* transform) const;


		static bool AABB(const glm::vec2& pos, const glm::vec2& bottomLeft, const glm::vec2 size);
	};
}