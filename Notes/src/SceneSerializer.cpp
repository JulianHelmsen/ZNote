#include "SceneSerializer.h"
#include <glm/gtc/type_ptr.hpp>

namespace app {

	void SceneSerializer::Serialize(const char* filepath, const Scene& scene) {
		FILE* file;
		fopen_s(&file, filepath, "wb");

		// write number of vertex buffer
		uint32_t sizeHeader = (uint32_t)scene.hostVertices.size();
		fwrite(&sizeHeader, sizeof(sizeHeader), 1, file);
		// write vertex buffer data
		fwrite(scene.hostVertices.data(), sizeof(Vertex), scene.hostVertices.size(), file);
		// write number of index buffer
		sizeHeader = (uint32_t)scene.hostIndices.size();
		fwrite(&sizeHeader, sizeof(sizeHeader), 1, file);
		// write index buffer data
		fwrite(scene.hostIndices.data(), sizeof(uint32_t), scene.hostIndices.size(), file);
		// write scale matrix
		fwrite(glm::value_ptr(scene.scaleMatrix), sizeof(glm::mat4), 1, file);
		// write translation matrix
		fwrite(glm::value_ptr(scene.translationMatrix), sizeof(glm::mat4), 1, file);


		fclose(file);
	}

	void SceneSerializer::Deserialize(Scene* scene, const char* filepath) {
		FILE* file;
		fopen_s(&file, filepath, "rb");
		uint32_t elemCount;
		// read number of vertices
		fread_s(&elemCount, sizeof(elemCount), sizeof(uint32_t), 1, file);
		// read vertex buffer
		std::vector<Vertex> newVertexBuffer;
		newVertexBuffer.resize(elemCount);
		fread_s(newVertexBuffer.data(), elemCount * sizeof(Vertex), sizeof(Vertex), elemCount, file);

		// read number of indices
		fread_s(&elemCount, sizeof(elemCount), sizeof(uint32_t), 1, file);

		// read index buffer
		std::vector<uint32_t> newIndexBuffer;
		newIndexBuffer.resize(elemCount);
		fread_s(newIndexBuffer.data(), elemCount * sizeof(uint32_t), sizeof(uint32_t), elemCount, file);

		// read scale matrix
		fread_s(glm::value_ptr(scene->scaleMatrix), sizeof(scene->scaleMatrix), sizeof(scene->scaleMatrix), 1, file);

		// read translation matrix
		fread_s(glm::value_ptr(scene->translationMatrix), sizeof(scene->translationMatrix), sizeof(scene->translationMatrix), 1, file);

		scene->hostVertices = newVertexBuffer;
		scene->hostIndices = newIndexBuffer;
		fclose(file);
	}
}