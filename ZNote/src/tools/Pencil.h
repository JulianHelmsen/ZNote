#include "Tool.h"

namespace app {


	class Pencil : public Tool {
	public:
		Pencil();
		void OnDrag(const glm::vec2& prev, const glm::vec2& newpos, int button) override;
		void OnButtonStateChanged(MouseButton button, const glm::vec2& pos, bool down) override;
		void OnKeyPress(uint32_t keycode) override;

		inline Color GetColor() const { return colorPallete[colorPalleteIdx]; }
	private:
		void AddLineSegment(const glm::vec2& newpos);
		bool m_newStroke;
		Color colorPallete[9];
		static uint32_t colorPalleteIdx;

	};
}