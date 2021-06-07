#pragma once

#include "core/Layer.h"

namespace app {


	class GuiLayer : public Layer {
	public:

		~GuiLayer() override;

		virtual void OnAttach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnDetach() override;

		virtual void OnUpdate() override;
	private:
	};
}