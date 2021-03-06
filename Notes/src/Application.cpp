#include "Application.h"
#include <GL/glew.h>
#include "renderer/Shader.h"
#include "renderer/RenderDefaults.h"
#include "os/Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Keycodes.h"
#include "SceneSerializer.h"
#include "Pencil.h"
#include "Eraser.h"

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

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // color

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*) (sizeof(float) * 2));

		
		m_program = utils::CreateShaderProgram();
		glUseProgram(m_program);
		
		
		m_uniformLocationViewProjection = glGetUniformLocation(m_program, "viewProjectionMatrix");
		glUniformMatrix4fv(m_uniformLocationViewProjection, 1, GL_FALSE, glm::value_ptr(m_scene.scaleMatrix));

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
		glClear(GL_COLOR_BUFFER_BIT);

		// calculate and set new projection matrix
		m_scene.viewProjectionMatrix = m_scene.scaleMatrix * m_scene.translationMatrix;
		glUniformMatrix4fv(m_uniformLocationViewProjection, 1, GL_FALSE, glm::value_ptr(m_scene.viewProjectionMatrix));

		// copy host index and vertex buffer to gpu
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) m_scene.hostVertices.size() * sizeof(Vertex), m_scene.hostVertices.data(), GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)m_scene.hostIndices.size() * sizeof(uint32_t), m_scene.hostIndices.data(), GL_DYNAMIC_DRAW);

		// Draw
		constexpr GLenum primitive = GL_LINES;
		glDrawElements(primitive, (GLsizei) m_scene.hostIndices.size(), GL_UNSIGNED_INT, NULL);

#if 0
		uint32_t vramUsage = (uint32_t) (m_scene.hostIndices.size() * sizeof(uint32_t) + m_scene.hostVertices.size() * sizeof(Vertex));
		printf("vram usage: %d\n", vramUsage);
#endif
	}

	void Application::Run() {
		while (!Window::IsClosed()) {
			Update();
			Window::PollEvents();
			Window::SwapBuffers();
		}
	}

	void Application::OnClose() {
		glDeleteProgram(m_program);
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ibo);
		glDeleteVertexArrays(1, &m_vao);

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
