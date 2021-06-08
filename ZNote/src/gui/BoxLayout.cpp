#include "BoxLayout.h"


namespace gui {

	void BoxLayout::Revalidate() {
		float xOffset = GetX();
		float yOffset = GetY();

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
			}else if (m_direction == Direction::Y_AXIS) {
				x -= child->GetWidth() * 0.5f;
				yOffset += child->GetHeight(); // advance
			}

			child->SetPosition(x, y);
			child->Invalidate();
		}


		// resize container to fit all children in it
		if (m_direction == Direction::X_AXIS && xOffset - GetX() > m_bounds.size.x) {
			m_bounds.size.x = xOffset - GetX();
		}else if(m_direction == Direction::Y_AXIS && yOffset - GetY() > m_bounds.size.y) {
			m_bounds.size.y = yOffset - GetY();
		}
	}

}