#include "GuiComponent.h"



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

	void GuiComponent::SetSize(float width, float height) {
		m_bounds.size = glm::vec2(width, height);
	}

	void GuiComponent::SetPosition(float x, float y) {
		m_bounds.position = glm::vec2(x, y);
	}


	void GuiComponent::Delete() {
		for (GuiComponent* child : m_children)
			delete child; // calls destructor of child which will destroy it's children
		
	}

	void GuiComponent::Revalidate() {
		for (GuiComponent* child : m_children)
			child->Revalidate();
	}

	void GuiComponent::Draw() const {
		for (GuiComponent* child : m_children)
			child->Draw();
	}
}