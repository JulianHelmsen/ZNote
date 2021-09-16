#pragma once

#include "core/Layer.h"
#include "Scene.h"
#include <glm/glm.hpp>

namespace app {

	/*
	* The canvas layer is supposed to handle user input which is supposed to change the scene
	* Changing the tools and such things should be done by a gui-layer
	*/
	class CanvasLayer : public Layer {

		~CanvasLayer() override;

		virtual void OnAttach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnDetach() override;

		virtual void OnUpdate() override;

	private:

		MouseButton m_pressedButton = MouseButton::NONE;

		void Zoom(float zoom, const glm::vec2& mousePosition);
		void Zoom(float zoom);

		void OnDrag(const MouseButton& button, const MouseMoved& moved);
		void OnScroll(const MouseScrolled& event);
		void OnKeyPress(const KeyPressed& event);
		void OnButtonStateChanged(Event& event);


		void AddImage(Image& image);
	};
}