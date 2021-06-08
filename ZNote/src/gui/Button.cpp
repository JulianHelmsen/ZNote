#include "Button.h"
#include "renderer/Renderer2D.h"
#include <core/Logger.h>
#include <cassert>



namespace gui {

	Button::Button() {
		m_toRender = true;
		m_color = app::Color(rand() % 255, rand() % 255, rand() % 255);
	}

	bool Button::Clicked() {
		ASSERT(m_toRender);

		if (m_clickCallback)
			m_clickCallback();

		return true;
	}


	
}