#pragma once

#include <vector>
#include "renderer/Batch.h"
#include "Color.h"
#include <glm/glm.hpp>
#include <string>


namespace app {

	struct Vertex {
		glm::vec2 position;
		Color color;
	};


	struct Image {
		glm::vec2 centerPos;
		glm::vec2 size;
		
		uint32_t textureId;
		std::string filepath;
	};


	struct Scene {
		renderer::Batch<Vertex> lineBatch;
		std::vector<Image> images;
		std::string filepath;

		glm::mat4 viewMatrix = glm::mat4(1.0f);
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);

		static void CleanUp(Scene& scene);

	};
}