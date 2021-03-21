#pragma once

#include <glm/glm.hpp>
#include "Batch.h"


namespace app {



	class Renderer2D {
	public:
		static void Initialize();
		static void CleanUp();

		static void Begin(const glm::mat4& projection);

		template<typename T>
		static void DrawBatch(const renderer::Batch<T>& lineBatch);

		static void DrawImage(uint32_t rendererId, glm::vec2& position, const glm::vec2& size);
		static void End();
	private:
		static void FlushImageBatch();
	};
}