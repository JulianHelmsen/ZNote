#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <stdint.h>
#include "Color.h"


namespace gui {

	struct BoundingBox {
		glm::vec2 position;
		glm::vec2 size;
	};



	class GuiComponent {
	public:
		~GuiComponent() { Delete(); }
		void Delete();
		
		virtual void DrawAndRevalidate();

		/*
		* returns whether a gui component has consumed this event
		*/
		virtual bool Clicked() { return false; }

		/*
		* Checks for bounding box and calls Clicked if position is inside bounding box
		* returns whether a gui component has consumed the event
		*/
		bool CheckForMouseClick(const glm::vec2& position);
		bool IsOverGui(const glm::vec2& position) const;

		void Invalidate() { m_valid = false; }
		virtual void Revalidate() {}
		bool IsValid() { return m_valid; }

		void SetBounds(float left, float bottom, float width, float height);
		void SetSize(float width, float height);
		void SetPosition(float x, float y);

		inline float GetWidth() const { return m_bounds.size.x; }
		inline float GetHeight() const { return m_bounds.size.y; }
		inline float GetX() const { return m_bounds.position.x; }
		inline float GetY() const { return m_bounds.position.y; }
		
		bool IsVisible() const { return m_visible; }
		void SetVisible(bool visible);
		void AddChild(GuiComponent* child);
	protected:
		bool m_visible = false;
		app::Color m_color;

		bool BoundingBoxContains(const glm::vec2& position) const;

		
		GuiComponent* m_parent = NULL;
		std::vector<GuiComponent*> m_children;
		bool m_drawable = false;

		BoundingBox m_bounds;
	private:

		// whether the component is laid out correcly
		bool m_valid;

	};
}