#include "BoxLayout.h"


namespace gui {


	void BoxLayout::OnRevalidate() {
		float xOffset = GetX();
		float yOffset = GetY();


		float newWidth = 0.0f;
		float newHeight = 0.0f;


		if (m_direction == Direction::X_AXIS) {
			if (m_alignment == Alignment::CENTER) {
				yOffset += GetHeight() * 0.5f;
			}
		}else if(m_direction == Direction::Y_AXIS) {
			if (m_alignment == Alignment::CENTER) {
				xOffset += GetWidth() * 0.5f;
			}
		}

		for (GuiComponent* child : m_children) {
			if (!child->IsVisible()) {
				continue;
			}
			// position children
			// calculate bottom left position
			float x = xOffset;
			float y = yOffset;

			if (m_direction == Direction::X_AXIS) {
				y -= child->GetHeight() * 0.5f;
				xOffset += child->GetWidth(); // advance
				newHeight = fmax(child->GetHeight(), newHeight);
				
				if (y < GetY())
					y = GetY();

			}else if (m_direction == Direction::Y_AXIS) {
				x -= child->GetWidth() * 0.5f;
				newWidth = fmax(child->GetWidth(), newWidth);
				if (x < GetX())
					x = GetX();
				yOffset += child->GetHeight(); // advance
			}

			child->SetPosition(x, y);
			child->Invalidate();
		}

		glm::vec2 newSize = m_bounds.size;
		if(!m_fixedSize)
			if (m_direction == Direction::X_AXIS) {
				newSize.x = xOffset - GetX();
				newSize.y = newHeight;
			}else {
				newSize.x = newWidth;
				newSize.y = yOffset - GetY();
			}

		// careful: floating point comparision
		if (newSize != m_bounds.size) {
			m_bounds.size = newSize;
			if(m_parent)
				m_parent->Invalidate();
		}
}

}