#include "GuiComponent.h"
#include <renderer/Renderer2D.h>


namespace gui {

	void GuiComponent::SetBounds(float left, float bottom, float width, float height) {
		m_bounds.position = glm::vec2(left, bottom);
		m_bounds.size = glm::vec2(width, height);
	}

	void GuiComponent::AddChild(GuiComponent* child) {
		// This component becomes the childs parent
		child->m_parent = this;
		m_children.push_back(child);

		Invalidate();
	}


	bool GuiComponent::IsOverGui(const glm::vec2& position) const {
		if (m_visible)
			return BoundingBoxContains(position);
		
		for (const GuiComponent* child : m_children)
			if (child->IsOverGui(position))
				return true;
		return false;
	}

	void GuiComponent::SetSize(float width, float height) {
		m_bounds.size = glm::vec2(width, height);
	}

	void GuiComponent::SetPosition(float x, float y) {
		m_bounds.position = glm::vec2(x, y);
	}

	void GuiComponent::SetVisible(bool visible) {
		m_visible = visible;
		if(m_parent)
			m_parent->Invalidate();
	}

	void GuiComponent::Delete() {
		for (GuiComponent* child : m_children)
			delete child; // calls destructor of child which will destroy it's children
		
	}


	void GuiComponent::DrawAndRevalidate() {
		if (!m_valid)
			Revalidate();

		if(m_visible)
			app::Renderer2D::DrawRect(m_bounds.position, m_bounds.size, m_color);

		for (GuiComponent* child : m_children)
			child->DrawAndRevalidate();
	}


	bool GuiComponent::BoundingBoxContains(const glm::vec2& position) const {
		return (position.x >= m_bounds.position.x && position.y >= m_bounds.position.y && position.x <= m_bounds.position.x + m_bounds.size.x && position.y <= m_bounds.position.y + m_bounds.size.y);
	}

	bool GuiComponent::CheckForMouseClick(const glm::vec2& position) {
		if (BoundingBoxContains(position)) {
			// children before parent
			for (GuiComponent* child : m_children) {
				if (child->CheckForMouseClick(position))
					return true;
			}

			return Clicked();
		}
		return false;
	}
}