#pragma once

#include "core/Layer.h"
#include "gui/GuiComponent.h"

namespace app {


	class GuiLayer : public Layer {
	public:

		~GuiLayer() override;

		virtual void OnAttach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnDetach() override;

		virtual void OnUpdate() override;
	private:


		void OnResize(uint32_t newWidth, uint32_t newHeight);
		gui::GuiComponent* m_root;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_inverseProjectionMatrix;

		uint32_t m_bucketTextureId;
		uint32_t m_pencilTextureId;
		uint32_t m_eraserTextureId;
		uint32_t m_transformTextureId;
		uint32_t m_colorButtonTextureId;
	};
}