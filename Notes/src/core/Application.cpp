#include "Application.h"
#include <GL/glew.h>
#include "renderer/RenderDefaults.h"
#include "os/Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Keycodes.h"
#include "SceneSerializer.h"
#include "tools/Pencil.h"
#include "tools/Eraser.h"
#include "tools/TransformTool.h"
#include "renderer/Renderer2D.h"
#include "renderer/TextureLoader.h"
#include <stdio.h>

namespace app {

	Application* Application::s_app = NULL;

	glm::vec2 Normalize(uint32_t x, uint32_t y) {
		glm::vec2 pos;
		pos.x = (float)x / Window::GetWidth() * 2 - 1;
		pos.y = (float)y / Window::GetHeight() * 2 - 1;
		pos.y = -pos.y;
		return pos;
	}


	
	Application::Application() {
		s_app = this;
		Window::SetDragCallback(std::bind(&Application::OnMouseDragged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
		Window::SetResizeCallback(std::bind(&Application::OnResize, this));
		Window::SetScrollWheelCallback(std::bind(&Application::OnScroll, this, std::placeholders::_1));
		Window::SetKeyCallback(std::bind(&Application::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2));
		Window::SetMouseButtonCallback(std::bind(&Application::OnMouseButtonStateChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
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


	void Application::OnResize() {
		uint32_t width = Window::GetWidth();
		uint32_t height = Window::GetHeight();
		glViewport(0, 0, width, height);
		const float aspectRatio = (float) width / height;
		constexpr float normalizedHalfHeight = 1.0f;
		const float normalizedHalfWidth = aspectRatio * normalizedHalfHeight;
		m_projectionMatrix = glm::ortho(-normalizedHalfWidth, normalizedHalfWidth, -normalizedHalfHeight, normalizedHalfHeight);
	}


	void Application::OnMouseDragged(uint32_t oldX, uint32_t oldY, uint32_t x, uint32_t y, int button) {
		glm::mat4 inverse = glm::inverse(m_viewProjectionMatrix);
		glm::vec2 normalizedOld = inverse * glm::vec4(Normalize(oldX, oldY), 0.0f, 1.0f);
		glm::vec2 normalized = inverse * glm::vec4(Normalize(x, y), 0.0f, 1.0f);

		if (button == 1) {
			m_scene.translationMatrix[3][0] -= normalizedOld.x - normalized.x;
			m_scene.translationMatrix[3][1] -= normalizedOld.y - normalized.y;
		}else if (m_currentTool && button == 0) {
			m_currentTool->OnDrag(normalizedOld, normalized, button);
		}			
	}

	void Application::OnMouseButtonStateChanged(MouseButton button, uint32_t x, uint32_t y, bool isdown) {
		glm::mat4 inverse = glm::inverse(m_viewProjectionMatrix);
		glm::vec2 mousePos = inverse * glm::vec4(Normalize(x, y), 0.0f, 1.0f);
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


	void Application::OnKeyPress(uint32_t keycode, uint32_t mods) {
		if (mods & KEY_MOD_CONTROL && keycode == KEY_S)
			Save();
		else if (mods & KEY_MOD_CONTROL && keycode == KEY_O)
			Load();
		else if (keycode == KEY_E)
			UseTool(new Eraser);
		else if (keycode == KEY_P)
			UseTool(new Pencil);
		else if (keycode == KEY_T)
			UseTool(new TransformTool);
		else if (keycode == KEY_I) {
			// load image and add it to scene
			std::optional<std::string> filepath = os::ShowOpenDialog(NULL);
			if (filepath) {
				Image image;
				image.textureId = utils::TextureLoader::LoadTexture(filepath->c_str(), &image.size);
				float aspectRatio = image.size.x / image.size.y;
				image.centerPos = -glm::vec2(m_scene.translationMatrix[3]);

				float height = 0.5f / m_scene.scaleMatrix[0][0];
				float width = aspectRatio * height;
				image.size.x = width;
				image.size.y = height;
				image.filepath = *filepath;
				m_scene.images.push_back(image);
			}
		}
		else if (m_currentTool)
			m_currentTool->OnKeyPress(keycode);
		
		
		
	}

	void Application::OnScroll(int dir) {
		if (dir > 0)
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
