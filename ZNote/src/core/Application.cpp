#include "Application.h"
#include <GL/glew.h>
#include "renderer/RenderDefaults.h"
#include "os/Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Keycodes.h"
#include "serialization/SceneSerializer.h"
#include "tools/Pencil.h"
#include "tools/Eraser.h"
#include "tools/TransformTool.h"
#include "renderer/Renderer2D.h"
#include "renderer/TextureLoader.h"

namespace app {

	Application* Application::s_app = NULL;



	
	Application::Application() {
		s_app = this;
		Window::SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		os::Clipboard::SetDataCallback<os::ClipboardImage>(std::bind(&Application::ClipboardImagePasted, this, std::placeholders::_1));

		Window::Create();
		glewInit();
		renderer::SetRenderDefaults();
		Renderer2D::Initialize();


		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		UseTool(new Pencil);

		
	}



	void Application::UseTool(Tool* tool) {
		if (m_currentTool)
			delete m_currentTool;
		
		m_currentTool = tool;
		tool->SetContext(&m_scene);
	}


	void Application::OnEvent(app::Event& event) {
		if (event.IsOfType<MousePressed>() ||event.IsOfType<MouseReleased>()) {
			OnMouseButtonStateChanged(event);
		}else if (event.IsOfType<MouseDragged>()) {
			OnMouseDragged(event.Get<MouseDragged>());
		}else if (event.IsOfType<WindowResized>()) {
			OnResize(event.Get<WindowResized>());
		}else if (event.IsOfType<MouseScrolled>()) {
			OnScroll(event.Get<MouseScrolled>());
		}else if (event.IsOfType<KeyPressed>()) {
			OnKeyPress(event.Get<KeyPressed>());

		}
	}


	void Application::OnResize(const WindowResized& resized) {
		glViewport(0, 0, resized.newWidth, resized.newHeight);
		const float aspectRatio = (float)resized.newWidth / resized.newHeight;
		constexpr float normalizedHalfHeight = 1.0f;
		const float normalizedHalfWidth = aspectRatio * normalizedHalfHeight;
		m_projectionMatrix = glm::ortho(-normalizedHalfWidth, normalizedHalfWidth, -normalizedHalfHeight, normalizedHalfHeight);
	}


	void Application::OnMouseDragged(MouseDragged& event) {
		glm::mat4 inverse = glm::inverse(m_viewProjectionMatrix);
		glm::vec2 normalizedOld = inverse * glm::vec4(Window::NormalizeScreenCoordinates(event.startX, event.startY), 0.0f, 1.0f);
		glm::vec2 normalized = inverse * glm::vec4(Window::NormalizeScreenCoordinates(event.endX, event.endY), 0.0f, 1.0f);

		if (event.button == MouseButton::RIGHT) {
			m_scene.translationMatrix[3][0] -= normalizedOld.x - normalized.x;
			m_scene.translationMatrix[3][1] -= normalizedOld.y - normalized.y;
		}else if (m_currentTool && event.button == MouseButton::LEFT) {
			m_currentTool->OnDrag(event.button, normalizedOld, normalized);
		}			
	}

	void Application::OnMouseButtonStateChanged(Event& event) {
		uint32_t x;
		uint32_t y;
		MouseButton button;
		bool isdown;

		if (event.IsOfType<MousePressed>()) {
			isdown = true;
			MousePressed& e = event.Get<MousePressed>();
			x = e.mouseX;
			y = e.mouseY;
			button = e.button;
		}else {
			isdown = false;
			MouseReleased& e = event.Get<MouseReleased>();
			x = e.mouseX;
			y = e.mouseY;
			button = e.button;
		}
		glm::mat4 inverse = glm::inverse(m_viewProjectionMatrix);
		glm::vec2 mousePos = inverse * glm::vec4(Window::NormalizeScreenCoordinates(x, y), 0.0f, 1.0f);
		if (m_currentTool)
			m_currentTool->OnButtonStateChanged(button, mousePos, isdown);
	}

	void Application::Update() {
		// clear screen buffer
		m_scene.viewMatrix = m_scene.scaleMatrix * m_scene.translationMatrix;
		m_viewProjectionMatrix = m_projectionMatrix * m_scene.viewMatrix;
		glClear(GL_COLOR_BUFFER_BIT);
		Renderer2D::Begin(m_viewProjectionMatrix);
		for (const Image& image : m_scene.images) {
			Renderer2D::DrawImage(image.textureId, image.centerPos - image.size * 0.5f, image.size);
		}
		Renderer2D::End();

		Renderer2D::Begin(m_viewProjectionMatrix);
		Renderer2D::DrawBatch(m_scene.lineBatch);		
		Renderer2D::End();

		if (m_currentTool->WantsToBeDrawn())
			m_currentTool->Draw();
	}

	void Application::Run() {
		while (!Window::IsClosed()) {
			Update();
			Window::PollEvents();
			Window::SwapBuffers();
		}
	}

	void Application::OnClose() {
		Scene::CleanUp(m_scene);
		Renderer2D::CleanUp();

		Window::Destroy();
		if (m_currentTool)
			delete m_currentTool;
	}


	void Application::OnKeyPress(const KeyPressed& event) {
		if (event.mods & KEY_MOD_CONTROL && event.keycode == KEY_S)
			Save();
		else if (event.mods & KEY_MOD_CONTROL && event.keycode == KEY_O)
			Load();
		else if (event.mods & KEY_MOD_CONTROL && event.keycode == KEY_V) {
			os::Clipboard::Enumerate();
		}
		else if (event.keycode == KEY_E)
			UseTool(new Eraser);
		else if (event.keycode == KEY_P)
			UseTool(new Pencil);
		else if (event.keycode == KEY_T)
			UseTool(new TransformTool);
		else if (event.keycode == KEY_I) {
			// load image and add it to scene
			std::optional<std::string> filepath = os::ShowOpenDialog(NULL);
			if (filepath) {
				Image image;
				image.textureId = utils::TextureLoader::LoadTexture(filepath->c_str(), &image.size);
				image.filepath = *filepath;
				AddImage(image);
				
			}
		}
		else if (m_currentTool)
			m_currentTool->OnKeyPress(event.keycode);
		
		
		
	}

	void Application::ClipboardImagePasted(const os::ClipboardImage& clipboardImage) {
		Image image;
		image.textureId = utils::TextureLoader::LoadTexture(clipboardImage.imageData, clipboardImage.width, clipboardImage.height, clipboardImage.numChannels);
		image.filepath = std::string("(none)");
		image.size.x = (float) clipboardImage.width;
		image.size.y = (float) clipboardImage.height;
		AddImage(image);
	}

	void Application::AddImage(Image& image) {
		float aspectRatio = image.size.x / image.size.y;
		image.centerPos = -glm::vec2(m_scene.translationMatrix[3]);

		float height = 0.5f / m_scene.scaleMatrix[0][0];
		float width = aspectRatio * height;
		image.size.x = width;
		image.size.y = height;
		m_scene.images.push_back(image);
	}

	void Application::OnScroll(const MouseScrolled& event) {
		if (event.direction > 0)
			m_scene.scaleMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.1f));
		else
			m_scene.scaleMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.9f));
	}

	void Application::Save() {
		std::optional<std::string> filepath = os::ShowSaveDialog();
		if (filepath) {
			SceneSerializer::Serialize(filepath->c_str(), m_scene);
		}
	}

	void Application::Load() {
		std::optional<std::string> filepath = os::ShowOpenDialog(NULL);
		if (filepath) {
			SceneSerializer::Deserialize(&m_scene, filepath->c_str());
		}
	}
}
