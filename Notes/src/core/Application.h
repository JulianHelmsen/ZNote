#pragma once

#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>
#include "core/Window.h"
#include "Scene.h"
#include "tools/Tool.h"

namespace app {




	

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

		void Update();
		void OnMouseDragged(uint32_t oldX, uint32_t oldY, uint32_t x, uint32_t y, int button);
		void OnMouseButtonStateChanged(MouseButton button, uint32_t x, uint32_t y, bool isdown);
		void OnResize();
		void OnKeyPress(uint32_t keycode, uint32_t mods);
		void OnScroll(int dir);
		void UseTool(Tool* tool);
		
		
		void Save();
		void Load();

		Scene m_scene;
		Tool* m_currentTool = NULL;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewProjectionMatrix;
		
	};
}