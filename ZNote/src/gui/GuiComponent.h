#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <stdint.h>



namespace gui {

	struct BoundingBox {
		glm::vec2 position;
		glm::vec2 size;
	};



	class GuiComponent {
	public:
		~GuiComponent() { Delete(); }
		void Delete();
		virtual void Draw() const;

		void Invalidate() { m_valid = false; }
		virtual void Revalidate();
		virtual bool IsValid() { return m_valid; }

		void SetBounds(float left, float bottom, float width, float height);
		void SetSize(float width, float height);
		void SetPosition(float x, float y);

		inline float GetWidth() const { return m_bounds.size.x; }
		inline float GetHeight() const { return m_bounds.size.y; }
		inline float GetX() const { return m_bounds.position.x; }
		inline float GetY() const { return m_bounds.position.y; }

		void AddChild(GuiComponent* child);
	protected:


		
		GuiComponent* m_parent = NULL;
		std::vector<GuiComponent*> m_children;
		bool m_drawable = false;

		BoundingBox m_bounds;
	private:

		// whether the component is laid out correcly
		bool m_valid;

	};
}