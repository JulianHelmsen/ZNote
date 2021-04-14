#include "TransformTool.h"

namespace app {


	bool TransformTool::AABB(const glm::vec2& pos, const glm::vec2& bottomLeft, const glm::vec2 size) {
		bool b1 = pos.x > bottomLeft.x;
		bool b2 = pos.x < bottomLeft.x + size.x;
		bool b3 = pos.y > bottomLeft.y;
		bool b4 = pos.y < bottomLeft.y + size.y;
		return b1 && b2 && b3 && b4;
	}


	void TransformTool::OnButtonStateChanged(MouseButton button, const glm::vec2& mousePos, bool down) {

		if (down) {
			Image* newTarget = NULL;
			for (Image& image : m_context->images) {
				const glm::vec2 bottomLeft = image.centerPos - image.size * 0.5f;
				if (AABB(mousePos, bottomLeft, image.size)) {
					newTarget = &image;
					break;
				}
			}
			if (newTarget) {
				SelectImage(*newTarget);
			}else {
				m_drawIcon = false;
			}
		}
	}




	void TransformTool::Draw() const {
		// render guizmo if element is selected
		if (m_target) {

		}
	}

	void TransformTool::SelectImage(Image& image) {
		m_target = &image;
		m_targetType = TargetType::IMAGE;
		m_drawIcon = true;
		printf("Selected image: %s\n", image.filepath.c_str());
	}

	template<>
	void TransformTool::GetTransform<TransformTool::TargetType::IMAGE>(glm::mat4* transform) const {
		const Image* image = (Image*) m_target;
		

	}
}