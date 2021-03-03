#include "Application.h"
#include "Window.h"
#include <GL/glew.h>
#include "Shader.h"
#include "RenderDefaults.h"
#include <glm/gtc/matrix_transform.hpp>

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
		m_currentColor = BLUE;
		translationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);

		m_uniformLocationViewProjection = glGetUniformLocation(m_program, "viewProjectionMatrix");
		glUniformMatrix4fv(m_uniformLocationViewProjection, 1, GL_FALSE, &scaleMatrix[0][0]);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	}

	void Application::OnResize() {
		glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
	}

	void Application::OnMouseDragged(uint32_t oldX, uint32_t oldY, uint32_t x, uint32_t y, int button) {
		glm::mat4 inverse = glm::inverse(viewProjectionMatrix);
		glm::vec2 normalizedOld = inverse * glm::vec4(Normalize(oldX, oldY), 0.0f, 1.0f);
		glm::vec2 normalized = inverse * glm::vec4(Normalize(x, y), 0.0f, 1.0f);

		if (button == 0) {
			m_hostIndices.push_back(m_hostVertices.size());
			m_hostVertices.push_back({ normalizedOld.x, normalizedOld.y, m_currentColor });
			m_hostIndices.push_back(m_hostVertices.size());
			m_hostVertices.push_back({ normalized.x, normalized.y, m_currentColor });
		}else if (button == 1) {
			translationMatrix[3][0] -= normalizedOld.x - normalized.x;
			translationMatrix[3][1] -= normalizedOld.y - normalized.y;
		}
		printf("%d\n", button);
	}


	void Application::Update() {
		glClear(GL_COLOR_BUFFER_BIT);
		viewProjectionMatrix = translationMatrix * scaleMatrix;
		glUniformMatrix4fv(m_uniformLocationViewProjection, 1, GL_FALSE, &viewProjectionMatrix[0][0]);
		// copy host index and vertex buffer to gpu
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) m_hostVertices.size() * sizeof(Vertex), m_hostVertices.data(), GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) m_hostIndices.size() * sizeof(uint32_t), m_hostIndices.data(), GL_DYNAMIC_DRAW);

		// Draw
		glDrawElements(GL_LINES, (GLsizei) m_hostIndices.size(), GL_UNSIGNED_INT, NULL);
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
	}
}
