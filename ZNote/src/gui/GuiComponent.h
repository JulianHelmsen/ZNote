#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <stdint.h>
#include "Color.h"
#include <functional>
#include "renderer/TextureLoader.h"


namespace gui {

	struct BoundingBox {
		glm::vec2 position{};
		glm::vec2 size{};
	};



	class GuiComponent {
	public:

		using ForEachChildFunction = std::function<void(uint32_t idx, GuiComponent* child)>;

		GuiComponent() : m_color(app::Color{ 255, 255, 255, 255 }), m_textureId(utils::TextureLoader::GetWhiteTexture()) {}
		~GuiComponent() { Delete(); }
		void Delete();
		
		virtual void DrawAndRevalidate();

		/*
		* returns whether a gui component has consumed this event
		*/
		virtual bool Clicked() { return false; }
		virtual bool Hovered(bool hovered) { return false; }

		/*
		* Checks for bounding box and calls Clicked if position is inside bounding box
		* returns whether a gui component has consumed the event
		*/
		bool CheckForMouseClick(const glm::vec2& position);
		bool IsOverGui(const glm::vec2& position) const;
		bool OnMouseMoved(const glm::vec2& position);

		void Invalidate() { m_valid = false; }
		void Revalidate();
		virtual void OnRevalidate() {}
		bool IsValid() { return m_valid; }

		void SetBounds(float left, float bottom, float width, float height);
		void SetSize(float width, float height);
		void SetPosition(float x, float y);
		

		inline float GetWidth() const { return m_bounds.size.x; }
		inline float GetHeight() const { return m_bounds.size.y; }
		inline float GetX() const { return m_bounds.position.x; }
		inline float GetY() const { return m_bounds.position.y; }
		
		bool IsVisible() const;
		void SetFixedSize(bool fixedSize) { m_fixedSize = fixedSize; }
		bool ShouldBeRendered() { return m_toRender; }
		void SetShouldBeRendered(bool visible);
		void AddChild(GuiComponent* child);
		virtual void SetColor(app::Color color) { m_color = color; }

		uint32_t GetChildCount() const { return (uint32_t) m_children.size(); }
		void ForEachChild(ForEachChildFunction func);
		void SetTexture(uint32_t texId) { m_textureId = texId; }
	protected:
		bool m_toRender = false;
		app::Color m_color;

		bool BoundingBoxContains(const glm::vec2& position) const;

		
		GuiComponent* m_parent = NULL;
		std::vector<GuiComponent*> m_children;
		bool m_drawable = false;
		bool m_fixedSize = false;

		BoundingBox m_bounds;
	private:

		// whether the component is laid out correcly
		bool m_valid;

		uint32_t m_textureId;
	};
}