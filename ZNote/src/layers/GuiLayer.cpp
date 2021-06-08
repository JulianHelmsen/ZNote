#include "gui/gui.h"
#include "GuiLayer.h"
#include <renderer/Renderer2D.h>
#include "core/Window.h"
#include "core/Logger.h"
#include <glm/gtc/matrix_transform.hpp>


namespace app {





	void GuiLayer::OnAttach() {
		m_root = new gui::GuiComponent;
		OnResize(Window::GetWidth(), Window::GetHeight());
		
		gui::BoxLayout* boxLayout = new gui::BoxLayout(gui::BoxLayout::Direction::Y_AXIS);
		boxLayout->SetAlignment(gui::BoxLayout::Alignment::CENTER);
		m_root->AddChild(boxLayout);
		boxLayout->SetPosition(0.0f, 0.0f);
		boxLayout->SetSize(0.1f, 0.4f);

		gui::Button* button1 = new gui::Button;
		gui::Button* button2 = new gui::Button;
		gui::Button* button3 = new gui::Button;
		gui::Button* button4 = new gui::Button;

		button1->SetSize(0.1f, 0.1f);
		button2->SetSize(0.1f, 0.1f);
		button3->SetSize(0.1f, 0.1f);
		button4->SetSize(0.1f, 0.1f);

		boxLayout->AddChild(button1);
		boxLayout->AddChild(button2);
		boxLayout->AddChild(button3);
		boxLayout->AddChild(button4);

		m_root->Revalidate();
	}

	void GuiLayer::OnEvent(Event& event) {
		if (event.IsOfType<MousePressed>()) {
			// check whether the mouse was pressed over a button
			MousePressed& pressed = event.Get<MousePressed>();
			glm::vec2 normalizedMousePos = m_inverseProjectionMatrix * glm::vec4(Window::NormalizeScreenCoordinates(pressed.mouseX, pressed.mouseY), 0.0f, 1.0f);

			bool hit = m_root->CheckForMouseClick(normalizedMousePos);
			// if the mouse clicked on a gui element the event is consumed
			if (hit)
				event.Handled();
		}else if (event.IsOfType<MouseReleased>()) {
			MouseReleased& released = event.Get<MouseReleased>();
			glm::vec2 normalizedMousePos = m_inverseProjectionMatrix * glm::vec4(Window::NormalizeScreenCoordinates(released.mouseX, released.mouseY), 0.0f, 1.0f);

			bool guiHit = m_root->IsOverGui(normalizedMousePos);
			if (guiHit)
				event.Handled();
		}else if (event.IsOfType<WindowResized>()) {
			const WindowResized& resized = event.Get<WindowResized>();
			OnResize(resized.newWidth, resized.newHeight);
		}
	}

	void GuiLayer::OnResize(uint32_t newWidth, uint32_t newHeight) {
		float aspectRatio = (float)newWidth / newHeight;

		float height = 1.0f;
		float width = aspectRatio * height;

		m_root->SetBounds(0.0f, 0.0f, width, height);
		m_projectionMatrix = glm::ortho(0.0f, width, 0.0f, height);
		m_inverseProjectionMatrix = glm::inverse(m_projectionMatrix);
	}

	void GuiLayer::OnDetach() {
		delete m_root;
	}

	void GuiLayer::OnUpdate() {
		Renderer2D::Begin(m_projectionMatrix);
		m_root->Draw();
		Renderer2D::End();
	}

	GuiLayer::~GuiLayer() {

	}

}