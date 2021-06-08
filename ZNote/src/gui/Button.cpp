#include "Button.h"
#include "renderer/Renderer2D.h"
#include "renderer/TextureLoader.h"


namespace gui {

	Button::Button() {
		m_visible = true;
		m_color = app::Color(rand() % 255, rand() % 255, rand() % 255);
	}

	bool Button::Clicked() {
		return true;
	}
}