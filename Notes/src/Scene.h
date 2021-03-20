#pragma once

#include <vector>
#include "renderer/Batch.h"
#include "Color.h"
#include <glm/glm.hpp>


namespace app {

	struct Vertex {
		glm::vec2 position;
		Color color;
	};


	struct Scene {
		renderer::Batch<Vertex> lineBatch;

		glm::mat4 viewProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);

	};
}