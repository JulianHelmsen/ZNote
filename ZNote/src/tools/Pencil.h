#pragma once
#include "Tool.h"

namespace app {


	class Pencil : public Tool {
	public:
		Pencil();
		void OnDrag(MouseButton button, const glm::vec2& prev, const glm::vec2& newpos) override;
		void OnButtonStateChanged(MouseButton button, const glm::vec2& pos, bool down) override;
		void OnKeyPress(uint32_t keycode) override;

		inline Color GetColor() const { return colorPallete[colorPalleteIdx]; }
		static uint32_t GetColorPalleteSize() { return sizeof(colorPallete) / sizeof(colorPallete[0]); }
		static Color* GetColorPallete() { return colorPallete; }
		static void SetSelectedColorIndex(uint32_t idx);
	private:
		static Color colorPallete[9];
		void AddLineSegment(const glm::vec2& newpos);
		bool m_newStroke;
		static uint32_t colorPalleteIdx;

	};
}