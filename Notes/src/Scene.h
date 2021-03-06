#pragma once

#include <vector>
#include "Color.h"
#include <glm/glm.hpp>


namespace app {

	struct Vertex {
		glm::vec2 position;
		Color color;
	};


	struct Scene {
		std::vector<uint32_t> hostIndices;
		std::vector<Vertex> hostVertices;

		glm::mat4 viewProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);

	};
}