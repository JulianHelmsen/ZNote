#include "gui/gui.h"
#include "GuiLayer.h"
#include <renderer/Renderer2D.h>


namespace app {





	void GuiLayer::OnAttach() {
		m_root = new gui::GuiComponent;
		m_root->SetBounds(-1.0f, -1.0f, 2.0f, 2.0f);
		
		gui::BoxLayout* boxLayout = new gui::BoxLayout(gui::BoxLayout::Direction::X_AXIS);
		m_root->AddChild(boxLayout);
		boxLayout->SetPosition(-1.0f, 0.0f);
		boxLayout->SetSize(0.2f, 0.4f);

		gui::Button* button1 = new gui::Button;
		gui::Button* button2 = new gui::Button;

		button1->SetSize(0.3f, 0.3f);
		button2->SetSize(0.3f, 0.3f);

		boxLayout->AddChild(button1);
		boxLayout->AddChild(button2);

		m_root->Revalidate();
	}

	void GuiLayer::OnEvent(Event& event) {

	}

	void GuiLayer::OnDetach() {
		delete m_root;
	}

	void GuiLayer::OnUpdate() {
		Renderer2D::Begin(glm::mat4(1.0f));
		// Render components in here

		m_root->Draw();
		
		Renderer2D::End();
	}

	GuiLayer::~GuiLayer() {

	}

}