#pragma once

#include <stdint.h>
#include <vector>
#include "Color.h"
#include <glm/glm.hpp>
#include "Window.h"

namespace app {




	struct Vertex {
		float x;
		float y;
		Color color;
	};

	class Application {
	public:
		Application();
		void Run();
		void OnClose();
		

	private:
		// Rendering buffers
		uint32_t m_vao;
		uint32_t m_vbo;
		uint32_t m_ibo;
		// shader program
		uint32_t m_program;
		uint32_t m_uniformLocationViewProjection;

		std::vector<Vertex> m_hostVertices;
		std::vector<uint32_t> m_hostIndices;


		void Update();
		void OnMouseDragged(uint32_t oldX, uint32_t oldY, uint32_t x, uint32_t y, int button);
		void OnMouseButtonStateChanged(MouseButton button, uint32_t x, uint32_t y, bool isdown);
		void OnResize();
		void OnScroll(int dir);
		inline Color GetColor() const { return colorPallete[colorPalleteIdx]; }
		
		void Save();
		void Load();

		uint32_t colorPalleteIdx = 0;
		glm::mat4 viewProjectionMatrix;
		glm::mat4 translationMatrix;
		glm::mat4 scaleMatrix;
		Color colorPallete[8];
	};
}