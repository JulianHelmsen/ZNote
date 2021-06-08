#pragma once


#include "GuiComponent.h"

namespace gui {

	class Button : public GuiComponent {
	public:
		Button();
		bool Clicked() override;
	};
}