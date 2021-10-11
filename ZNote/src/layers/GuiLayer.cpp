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
#include "gui/Animation.h"
#include "core/Application.h"


namespace app {





	void GuiLayer::OnAttach() {
		m_root = new gui::BoxLayout(gui::BoxLayout::Direction::X_AXIS);
		OnResize(Window::GetWidth(), Window::GetHeight());
		((gui::BoxLayout*)m_root)->SetAlignment(gui::BoxLayout::Alignment::CENTER);

		m_root->SetFixedSize(true);
		
		m_toolList = new gui::BoxLayout(gui::BoxLayout::Direction::Y_AXIS);
		m_toolList->SetAlignment(gui::BoxLayout::Alignment::EDGE);
		
		m_extendButton = new gui::Button();
		m_extendButton->SetSize(0.01f, 0.05f);
		m_extendButton->SetClickCallback(std::bind(&GuiLayer::OnExtendButtonClicked, this));
		
		
		m_root->AddChild(m_toolList);
		m_root->AddChild(m_extendButton);


		m_toolList->SetPosition(0.0f, 0.0f);
		m_toolList->SetSize(0.1f, 0.4f);

		m_colorList = new gui::BoxLayout(gui::BoxLayout::Direction::X_AXIS);
		m_colorList->SetSize(0.1f, 0.1f);
		

		
		gui::Button* paintButton = new gui::Button();
		gui::Button* eraseButton = new gui::Button();
		gui::Button* transformButton = new gui::Button();
		gui::Button* saveButton = new gui::Button();

		paintButton->SetSize(0.1f, 0.1f);
		eraseButton->SetSize(0.1f, 0.1f);
		transformButton->SetSize(0.1f, 0.1f);
		saveButton->SetSize(0.1f, 0.1f);

		// elements to tool list are added bottom to top

		gui::Button* colorSelectionButton = new gui::Button;
		colorSelectionButton->SetSize(0.1f, 0.1f);
		m_colorList->AddChild(colorSelectionButton);

		
		m_bucketTextureId = utils::TextureLoader::LoadTexture(s_bucketImage, 32, 32, 4).textureId;
		m_transformTextureId = utils::TextureLoader::LoadTexture(s_transformToolImage, 32, 32, 4).textureId;
		m_eraserTextureId = utils::TextureLoader::LoadTexture(s_eraserToolImage, 32, 32, 4).textureId;
		m_pencilTextureId = utils::TextureLoader::LoadTexture(s_pencilToolImage, 32, 32, 4).textureId;
		m_colorButtonTextureId = utils::TextureLoader::LoadTexture(s_colorButton, 32, 32, 4).textureId;
		m_saveButtonTextureId = utils::TextureLoader::LoadTexture(s_saveButton, 32, 32, 4).textureId;

		colorSelectionButton->SetTexture(m_colorButtonTextureId);

		colorSelectionButton->SetClickCallback(std::bind(&GuiLayer::CollapseOrExpandColorSelection, this));

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

		

		m_toolList->AddChild(saveButton);
		m_toolList->AddChild(transformButton);
		m_toolList->AddChild(eraseButton);
		m_toolList->AddChild(paintButton);
		m_toolList->AddChild(m_colorList);


		transformButton->SetTexture(m_transformTextureId);
		eraseButton->SetTexture(m_eraserTextureId);
		paintButton->SetTexture(m_pencilTextureId);
		saveButton->SetTexture(m_saveButtonTextureId);

		saveButton->SetClickCallback([]() -> void {
			Application::GetApp()->Save();
		});

		transformButton->SetClickCallback([]() -> void {
			Tool::UseTool(new TransformTool);
		});
		eraseButton->SetClickCallback([]() -> void {Tool::UseTool(new Eraser); });
		paintButton->SetClickCallback([]() -> void {Tool::UseTool(new Pencil); });



		m_extended = false;
		m_root->SetPosition(-0.1f, 0.0f);
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

	void GuiLayer::OnExtendButtonClicked() {
		// if this is clicked the color pallete is always collapsed

		bool toExtend = !m_extended;
		m_extended = toExtend;
		// destructor of animation should call: delete this;

		gui::Animation* animation = gui::Animation::Create(0.15f);// second
		animation->EnableAutoDeletion();
		
		gui::GuiComponent* comp = m_root;
		

		animation->OnUpdate([comp, toExtend](const gui::Animation::AnimationState& state) -> void {
			// move gui to the right to hide the tool list
			constexpr float toolListWidth = 0.1f;
			float posX;
			if (toExtend) {
				posX = -toolListWidth + state.progress * toolListWidth;
			}else {
				posX = -state.progress * toolListWidth;
			}

			
			comp->SetPosition(posX, 0.0f);
			comp->Invalidate();
			Window::PostEmptyEvent();// empty event for redrawing the window
		});

		animation->Start();

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

		m_root->SetSize(width, height);
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
		glDeleteTextures(1, &m_saveButtonTextureId);
	}

	void GuiLayer::OnUpdate() {
		Renderer2D::Begin(m_projectionMatrix);
		m_root->DrawAndRevalidate();
		Renderer2D::End();
	}

	GuiLayer::~GuiLayer() {

	}

}