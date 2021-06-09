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

	bool GuiComponent::OnMouseMoved(const glm::vec2& position) {
		bool contains = BoundingBoxContains(position);
		
		// children before parent
		bool isAnyChildHovered = false;
		for (GuiComponent* child : m_children) {
			if (child->OnMouseMoved(position))
				isAnyChildHovered |= true;
		}

		if (m_toRender && contains)
			return Hovered(true);
		
		Hovered(contains);
		return isAnyChildHovered;
	}

	bool GuiComponent::IsOverGui(const glm::vec2& position) const {
		if (m_toRender)
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

	void GuiComponent::SetShouldBeRendered(bool toRender) {
		m_toRender = toRender;
		if(m_parent)
			m_parent->Invalidate();
	}

	void GuiComponent::Delete() {
		for (GuiComponent* child : m_children)
			delete child; // calls destructor of child which will destroy it's children
		
	}

	bool GuiComponent::IsVisible() const {
		if (m_toRender)
			return true;
		for (GuiComponent* child : m_children)
			if (child->IsVisible())
				return true;
		return false;
	}

	void GuiComponent::DrawAndRevalidate() {
		if (!m_valid)
			Revalidate();

		if(m_toRender)
			app::Renderer2D::DrawImage(m_textureId, m_bounds.position, m_bounds.size, m_color);

		for (GuiComponent* child : m_children)
			child->DrawAndRevalidate();
	}

	void GuiComponent::ForEachChild(ForEachChildFunction func) {
		for (uint32_t i = 0; i < m_children.size(); i++) {
			func(i, m_children[i]);
		}
	}

	/*
	* AABB collision detection
	*/
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

			if(m_toRender)
				return Clicked();
		}
		return false;
	}
}