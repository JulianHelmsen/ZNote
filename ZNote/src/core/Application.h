#pragma once

#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>
#include "core/Window.h"
#include "Scene.h"
#include "tools/Tool.h"
#include "os/Clipboard.h"
#include "Event.h"
#include "Layer.h"

namespace app {




	

	class Application {
	public:
		Application();
		void Run();
		void OnClose();
		

		static const glm::mat4& GetViewMatrix() { return s_app->m_scene.viewMatrix; }
		static const glm::mat4& GetProjectionMatrix() { return s_app->m_projectionMatrix; }
		static const glm::mat4& GetViewProjectionMatrix() { return s_app->m_viewProjectionMatrix; }
		static Application* GetApp() { return s_app; }
		static Scene& GetActiveScene() { return s_app->m_scene; }
	private:
		static Application* s_app;
		// Rendering buffers
		uint32_t m_vao;
		uint32_t m_vbo;
		uint32_t m_ibo;
		// shader program
		uint32_t m_program;
		uint32_t m_uniformLocationViewProjection;

		void Update();

		void OnEvent(app::Event& event);

		void OnResize(const WindowResized&);
		void OnKeyPress(const KeyPressed& event);
		void ClipboardImagePasted(const os::ClipboardImage& image);
		void FilePasted(const os::FilePasted& paste);
		bool IsLayerStackEmpty() const { return m_layers.size() > 0; }

		void PushLayer(Layer* layer);
		void PopLayer();
		
		
		void Save();
		void Load();

		Scene m_scene;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewProjectionMatrix;

		std::vector<Layer*> m_layers;
		
	};
}