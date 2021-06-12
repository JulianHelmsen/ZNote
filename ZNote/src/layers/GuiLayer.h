#pragma once

#include "core/Layer.h"
#include "gui/gui.h"

namespace app {


	class GuiLayer : public Layer {
	public:

		~GuiLayer() override;

		virtual void OnAttach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnDetach() override;

		virtual void OnUpdate() override;


		void CollapseOrExpandColorSelection();

	private:


		void OnExtendButtonClicked();
		void OnResize(uint32_t newWidth, uint32_t newHeight);

		bool m_extended = true;

		gui::GuiComponent* m_root;
		gui::BoxLayout* m_colorList;
		gui::BoxLayout* m_toolList;
		gui::Button* m_extendButton;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_inverseProjectionMatrix;

		uint32_t m_bucketTextureId;
		uint32_t m_pencilTextureId;
		uint32_t m_eraserTextureId;
		uint32_t m_transformTextureId;
		uint32_t m_colorButtonTextureId;
	};
}