#include "GuiLayer.h"
#include <renderer/Renderer2D.h>
#include "core/Window.h"
#include "core/Logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include "tools/Pencil.h"
#include <tools/TransformTool.h>
#include <tools/Eraser.h>
#include "GuiLayerImages.h"
#include <GL/glew.h>


namespace app {





	void GuiLayer::OnAttach() {
		m_root = new gui::BoxLayout(gui::BoxLayout::Direction::X_AXIS);
		OnResize(Window::GetWidth(), Window::GetHeight());
		((gui::BoxLayout*)m_root)->SetAlignment(gui::BoxLayout::Alignment::CENTER);
		
		m_root->SetFixedSize(true);
		
		gui::BoxLayout* toolList = new gui::BoxLayout(gui::BoxLayout::Direction::Y_AXIS);
		toolList->SetAlignment(gui::BoxLayout::Alignment::EDGE);
		
		m_extendButton = new gui::Button();
		m_extendButton->SetSize(0.01f, 0.05f);
		
		
		m_root->AddChild(toolList);
		m_root->AddChild(m_extendButton);


		toolList->SetPosition(0.0f, 0.0f);
		toolList->SetSize(0.1f, 0.4f);

		m_colorList = new gui::BoxLayout(gui::BoxLayout::Direction::X_AXIS);
		m_colorList->SetSize(0.1f, 0.1f);
		

		
		gui::Button* paintButton = new gui::Button();
		gui::Button* eraseButton = new gui::Button();
		gui::Button* transformButton = new gui::Button();

		paintButton->SetSize(0.1f, 0.1f);
		eraseButton->SetSize(0.1f, 0.1f);
		transformButton->SetSize(0.1f, 0.1f);

		// elements to tool list are added bottom to top

		gui::Button* colorSelectionButton = new gui::Button;
		colorSelectionButton->SetSize(0.1f, 0.1f);
		m_colorList->AddChild(colorSelectionButton);

		
		m_bucketTextureId = utils::TextureLoader::LoadTexture(s_bucketImage, 32, 32, 4);
		m_transformTextureId = utils::TextureLoader::LoadTexture(s_transformToolImage, 32, 32, 4);
		m_eraserTextureId = utils::TextureLoader::LoadTexture(s_eraserToolImage, 32, 32, 4);
		m_pencilTextureId = utils::TextureLoader::LoadTexture(s_pencilToolImage, 32, 32, 4);
		m_colorButtonTextureId = utils::TextureLoader::LoadTexture(s_colorButton, 32, 32, 4);

		colorSelectionButton->SetTexture(m_colorButtonTextureId);

		colorSelectionButton->SetClickCallback([this]() -> void {
			this->CollapseOrExpandColorSelection();
		});

		for (uint32_t i = 0; i < Pencil::GetColorPalleteSize(); i++) {
			app::Color color = Pencil::GetColorPallete()[i];
			color.a = 240;

			gui::Button* colorButton = new gui::Button();
			colorButton->SetTexture(m_bucketTextureId);
			colorButton->SetShouldBeRendered(false);
			
			colorButton->SetClickCallback([i, this]() -> void {
				Pencil::SetSelectedColorIndex(i);
				this->CollapseOrExpandColorSelection();
			});

			colorButton->SetSize(0.1f, 0.1f);
			colorButton->SetColor(color);
			m_colorList->AddChild(colorButton);
		}

		m_colorList->Revalidate();


		toolList->AddChild(transformButton);
		toolList->AddChild(eraseButton);
		toolList->AddChild(paintButton);
		toolList->AddChild(m_colorList);


		transformButton->SetTexture(m_transformTextureId);
		eraseButton->SetTexture(m_eraserTextureId);
		paintButton->SetTexture(m_pencilTextureId);

		transformButton->SetClickCallback([]() -> void {
			Tool::UseTool(new TransformTool);
		});
		eraseButton->SetClickCallback([]() -> void {Tool::UseTool(new Eraser); });
		paintButton->SetClickCallback([]() -> void {Tool::UseTool(new Pencil); });


		toolList->Revalidate();

		m_root->Revalidate();
	}

	void GuiLayer::CollapseOrExpandColorSelection() {
		bool nowVisible;
		m_colorList->ForEachChild([&nowVisible](uint32_t idx, gui::GuiComponent* child)-> void {
			bool visible = child->ShouldBeRendered();
			if (idx == 1) {
				nowVisible = !visible;
			}
				
				

			if (idx > 0)
				child->SetShouldBeRendered(nowVisible);
		});

		m_extendButton->SetShouldBeRendered(!nowVisible);
	

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

			m_root->IsOverGui(normalizedMousePos);
		}else if (event.IsOfType<WindowResized>()) {
			const WindowResized& resized = event.Get<WindowResized>();
			OnResize(resized.newWidth, resized.newHeight);
		}
		else if (event.IsOfType<MouseMoved>()) {
			// check for hovered
			MouseMoved& moved = event.Get<MouseMoved>();
			glm::vec2 normalizedMousePos = m_inverseProjectionMatrix * glm::vec4(Window::NormalizeScreenCoordinates(moved.newX, moved.newY), 0.0f, 1.0f);
			m_root->OnMouseMoved(normalizedMousePos);
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
		glDeleteTextures(1, &m_bucketTextureId);
		glDeleteTextures(1, &m_transformTextureId);
		glDeleteTextures(1, &m_eraserTextureId);
		glDeleteTextures(1, &m_pencilTextureId);
		glDeleteTextures(1, &m_colorButtonTextureId);
	}

	void GuiLayer::OnUpdate() {
		Renderer2D::Begin(m_projectionMatrix);
		m_root->DrawAndRevalidate();
		Renderer2D::End();
	}

	GuiLayer::~GuiLayer() {

	}

}