#pragma once

#include <glm/glm.hpp>
#include "Batch.h"
#include "Color.h"


namespace app {



	class Renderer2D {
	public:
		static void Initialize();
		static void CleanUp();

		static void Begin(const glm::mat4& projection);

		template<typename T>
		static void DrawBatch(const renderer::Batch<T>& lineBatch);

		static void DrawImage(uint32_t rendererId, const glm::vec2& position, const glm::vec2& size, Color color = Color{ 255, 255, 255, 255 });
		static void DrawRect(const glm::vec2& position, const glm::vec2& size, Color color);
		static void End();


		// Draws a character with the Font::GetCurrentFont() font and returns the position of the next character
		static glm::vec2 DrawCharacter(char character, glm::vec2& pos, Color color = Color{255, 255, 255, 255});
	private:
		static void FlushImageBatch();
		static void FlushCharacterBatch();
	};
}