#include "Application.h"
#include <GL/glew.h>
#include "renderer/RenderDefaults.h"
#include "os/Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Keycodes.h"
#include "SceneSerializer.h"
#include "Pencil.h"
#include "Eraser.h"
#include "renderer/Renderer2D.h"
#include "renderer/TextureLoader.h"
#include <stdio.h>

namespace app {


	glm::vec2 Normalize(uint32_t x, uint32_t y) {
		glm::vec2 pos;
		pos.x = (float)x / Window::GetWidth() * 2 - 1;
		pos.y = (float)y / Window::GetHeight() * 2 - 1;
		pos.y = -pos.y;
		return pos;
	}


	Application::Application() {
		Window::Create();
		Window::SetDragCallback(std::bind(&Application::OnMouseDragged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
		Window::SetResizeCallback(std::bind(&Application::OnResize, this));
		Window::SetSaveCallback(std::bind(&Application::Save, this));
		Window::SetLoadCallback(std::bind(&Application::Load, this));
		Window::SetScrollWheelCallback(std::bind(&Application::OnScroll, this, std::placeholders::_1));
		Window::SetKeyCallback(std::bind(&Application::OnKeyPress, this, std::placeholders::_1));
		Window::SetMouseButtonCallback(std::bind(&Application::OnMouseButtonStateChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		glewInit();
		renderer::SetRenderDefaults();
		Renderer2D::Initialize();


		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		UseTool(new Pencil);

		for (int i = 0; i < 10; i++) {
			char buffer[1000];// 
			sprintf(buffer, "C:/Users/julia/Pictures/chess/first/%d.png", i);
			Image image;
			image.centerPos = glm::vec2(i * (1.1f), 0.0f);
			image.size = glm::vec2(1.0f, 0.5f);
			image.textureId = utils::TextureLoader::LoadTexture(buffer);
			m_scene.images.push_back(image);
		}
		

		Image image2;
		image2.centerPos = glm::vec2(2.0f, 0.0f);
		image2.size = glm::vec2(1.0f, 0.5f);
		image2.textureId = utils::TextureLoader::LoadTexture("C:/Users/julia/Pictures/backup/20190613_231720.jpg");
		m_scene.images.push_back(image2);
		
	}

	void Application::UseTool(Tool* tool) {
		if (m_currentTool)
			delete m_currentTool;
		
		m_currentTool = tool;
		tool->SetContext(&m_scene);
	}


	void Application::OnResize() {
		glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
	}


	void Application::OnMouseDragged(uint32_t oldX, uint32_t oldY, uint32_t x, uint32_t y, int button) {
		glm::mat4 inverse = glm::inverse(m_scene.viewProjectionMatrix);
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
		if (m_currentTool)
			m_currentTool->OnButtonStateChanged(button, isdown);
	}


	void Application::Update() {
		// clear screen buffer
		m_scene.viewProjectionMatrix = m_scene.scaleMatrix * m_scene.translationMatrix;
		glClear(GL_COLOR_BUFFER_BIT);
		Renderer2D::Begin(m_scene.viewProjectionMatrix);
		for (const Image& image : m_scene.images) {
			Renderer2D::DrawImage(image.textureId, image.centerPos - image.size * 0.5f, image.size);
		}
		Renderer2D::End();

		Renderer2D::Begin(m_scene.viewProjectionMatrix);
		Renderer2D::DrawBatch(m_scene.lineBatch);		
		Renderer2D::End();

		
	}

	void Application::Run() {
		while (!Window::IsClosed()) {
			Update();
			Window::PollEvents();
			Window::SwapBuffers();
		}
	}

	void Application::OnClose() {
		Renderer2D::CleanUp();

		Window::Destroy();
		if (m_currentTool)
			delete m_currentTool;
	}


	void Application::OnKeyPress(uint32_t keycode) {
		if (keycode == KEY_E)
			UseTool(new Eraser);
		else if (keycode == KEY_P)
			UseTool(new Pencil);
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
