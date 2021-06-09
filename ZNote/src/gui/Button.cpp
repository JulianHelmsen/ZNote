#include "Button.h"
#include "renderer/Renderer2D.h"
#include <core/Logger.h>
#include <cassert>



namespace gui {

	Button::Button()  {
		m_toRender = true;
		SetColor({ 255, 255, 255 });
	}

	bool Button::Clicked() {
		ASSERT(m_toRender);

		if (m_clickCallback)
			m_clickCallback();

		return true;
	}

	bool Button::Hovered(bool isHovered) {
		if (isHovered)
			m_color = m_hoverColor;
		else
			m_color = m_notHoveredColor;

		return isHovered;
	}

	void Button::SetColor(app::Color color) {
		GuiComponent::SetColor(color);
		m_hoverColor.r = color.r / 3 * 2;
		m_hoverColor.g = color.g / 3 * 2;
		m_hoverColor.b = color.b / 3 * 2;

		m_notHoveredColor = color;
	}


	
}