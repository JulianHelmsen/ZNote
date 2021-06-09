#pragma once


#include "GuiComponent.h"
#include <functional>


namespace gui {

	class Button : public GuiComponent {
	public:
		using Callback = std::function<void()>;

		Button();
		bool Clicked() override;

		void SetClickCallback(Callback callback) { m_clickCallback = callback; }
		bool Hovered(bool isHovered) override;
		void SetColor(app::Color color) override;
		
	private:
		Callback m_clickCallback;
		app::Color m_hoverColor, m_notHoveredColor;
		
	};
}