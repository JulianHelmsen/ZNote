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
		
	private:
		Callback m_clickCallback;
		
	};
}