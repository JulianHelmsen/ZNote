#include "Application.h"
#include <GL/glew.h>
#include "Shader.h"
#include "RenderDefaults.h"
#include "os/Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
		translationMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);

		m_uniformLocationViewProjection = glGetUniformLocation(m_program, "viewProjectionMatrix");
		glUniformMatrix4fv(m_uniformLocationViewProjection, 1, GL_FALSE, &scaleMatrix[0][0]);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		colorPallete[0] = WHITE;
		colorPallete[1] = PINK;
		colorPallete[2] = CORAL;
		colorPallete[3] = YELLOW;
		colorPallete[4] = TEAL;
		colorPallete[5] = LIGHT_BLUE;
		colorPallete[6] = BLUE;
		colorPallete[7] = PURPLE;
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
			m_hostVertices.push_back({ normalizedOld.x, normalizedOld.y, GetColor() });
			m_hostIndices.push_back(m_hostVertices.size());
			m_hostVertices.push_back({ normalized.x, normalized.y, GetColor() });
		}else if (button == 1) {
			translationMatrix[3][0] -= normalizedOld.x - normalized.x;
			translationMatrix[3][1] -= normalizedOld.y - normalized.y;
		}
	}

	void Application::OnMouseButtonStateChanged(MouseButton button, uint32_t x, uint32_t y, bool isdown) {
		if (button == MouseButton::WHEEL && isdown) {
			colorPalleteIdx++;
			if (colorPalleteIdx >= sizeof(colorPallete) / sizeof(Color))
				colorPalleteIdx = 0;
		}
	}


	void Application::Update() {
		// clear screen buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// calculate and set new projection matrix
		viewProjectionMatrix = scaleMatrix * translationMatrix;
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

	void Application::OnScroll(int dir) {
		if (dir > 0)
			scaleMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.1f));
		else
			scaleMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.9f));
	}

	void Application::Save() {
		std::optional<std::string> filepath = os::ShowSaveDialog();
		if (filepath) {
			FILE* file = fopen(filepath->c_str(), "wb");

			// write number of vertex buffer
			uint32_t sizeHeader = (uint32_t) m_hostVertices.size();
			fwrite(&sizeHeader, sizeof(sizeHeader), 1, file);
			// write vertex buffer data
			fwrite(m_hostVertices.data(), sizeof(Vertex), m_hostVertices.size(), file);
			// write number of index buffer
			sizeHeader = (uint32_t)m_hostIndices.size();
			fwrite(&sizeHeader, sizeof(sizeHeader), 1, file);
			// write index buffer data
			fwrite(m_hostIndices.data(), sizeof(uint32_t), m_hostIndices.size(), file);
			// write scale matrix
			fwrite(glm::value_ptr(scaleMatrix), sizeof(glm::mat4), 1, file);
			// write translation matrix
			fwrite(glm::value_ptr(translationMatrix), sizeof(glm::mat4), 1, file);
			

			fclose(file);
		}
	}

	void Application::Load() {
		std::optional<std::string> filepath = os::ShowOpenDialog(NULL);
		if (filepath) {
			FILE* file = fopen(filepath->c_str(), "rb");
			uint32_t elemCount;
			// read number of vertices
			fread_s(&elemCount, sizeof(elemCount), sizeof(uint32_t), 1, file);
			// read vertex buffer
			std::vector<Vertex> newVertexBuffer;
			newVertexBuffer.resize(elemCount);
			fread_s(newVertexBuffer.data(), elemCount * sizeof(Vertex), sizeof(Vertex), elemCount, file);

			// read number of indices
			fread_s(&elemCount, sizeof(elemCount), sizeof(uint32_t), 1, file);

			// read index buffer
			std::vector<uint32_t> newIndexBuffer;
			newIndexBuffer.resize(elemCount);
			fread_s(newIndexBuffer.data(), elemCount * sizeof(uint32_t), sizeof(uint32_t), elemCount, file);

			// read scale matrix
			fread_s(glm::value_ptr(scaleMatrix), sizeof(scaleMatrix), sizeof(scaleMatrix), 1, file);

			// read translation matrix
			fread_s(glm::value_ptr(translationMatrix), sizeof(translationMatrix), sizeof(translationMatrix), 1, file);

			m_hostVertices = newVertexBuffer;
			m_hostIndices= newIndexBuffer;
			fclose(file);
		}
	}
}
