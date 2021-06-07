#include "Button.h"
#include "renderer/Renderer2D.h"


namespace gui {


	void Button::Draw() const {
		app::Renderer2D::DrawImage(0, m_bounds.position, m_bounds.size);
	}
}