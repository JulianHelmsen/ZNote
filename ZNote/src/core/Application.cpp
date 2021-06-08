#include "Application.h"
#include <GL/glew.h>
#include "renderer/RenderDefaults.h"
#include "os/Utils.h"
#include "Keycodes.h"
#include "serialization/SceneSerializer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "tools/Pencil.h"
#include "tools/Eraser.h"
#include "tools/TransformTool.h"
#include "renderer/Renderer2D.h"
#include "renderer/TextureLoader.h"
#include "layers/CanvasLayer.h"
#include "layers/GuiLayer.h"

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

		Tool::UseTool(new Pencil);

		PushLayer(new CanvasLayer);
		PushLayer(new GuiLayer);
	}





	void Application::OnEvent(app::Event& event) {

		// last drawn layer is first to receive events
		for(int32_t i = (int32_t) m_layers.size() - 1; i >= 0; i--) {
			Layer* layer = m_layers[i];
			ASSERT(layer);
			layer->OnEvent(event);
			
			if (layer->CanBeDeleted()) {
				m_layers.erase(m_layers.begin() + i);
				i++; // next iteration => same index
			}
			
			if (event.IsHandled())
				return;
		}

		if (event.IsOfType<WindowResized>()) {
			OnResize(event.Get<WindowResized>());
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



	void Application::Update() {
		// recalculate matrices
		m_scene.viewMatrix = m_scene.scaleMatrix * m_scene.translationMatrix;
		m_viewProjectionMatrix = m_projectionMatrix * m_scene.viewMatrix;

		// clear screen buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// render all layers
		for (Layer* layer : m_layers) {
			layer->OnUpdate();
		}
	}

	
	void Application::OnClose() {
		Scene::CleanUp(m_scene);
		Renderer2D::CleanUp();

		while (!IsLayerStackEmpty())
			PopLayer();

		Window::Destroy();
		if (Tool::ActiveTool())
			delete Tool::ActiveTool();
	}


	void Application::OnKeyPress(const KeyPressed& event) {
		if (event.mods & KEY_MOD_CONTROL && event.keycode == KEY_S)
			Save();
		else if (event.mods & KEY_MOD_CONTROL && event.keycode == KEY_O)
			Load();
		else if (event.mods & KEY_MOD_CONTROL && event.keycode == KEY_V) {
			os::Clipboard::Enumerate();
		}
	}

	void Application::ClipboardImagePasted(const os::ClipboardImage& clipboardImage) {
		Event event;
		event.Set(clipboardImage);
		OnEvent(event);
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

	void Application::PushLayer(Layer* layer) {
		m_layers.push_back(layer);
		layer->OnAttach();
	}

	void Application::PopLayer() {
		if (m_layers.size() > 0) {
			Layer* layer = m_layers[m_layers.size() - 1];
			layer->OnDetach();
			delete layer;
			m_layers.pop_back();
		}
	}


	void Application::Run() {
		while (!Window::IsClosed()) {
			Update();
			Window::PollEvents();
			Window::SwapBuffers();
		}
	}

}
