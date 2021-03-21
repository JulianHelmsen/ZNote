#include "SceneSerializer.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

namespace app {

	void SceneSerializer::Serialize(const char* filepath, const Scene& scene) {
		FILE* file;
		fopen_s(&file, filepath, "wb");

		const std::vector<Vertex>& lineVertices = scene.lineBatch.GetVertexList();
		const std::vector<uint32_t>& lineIndices = scene.lineBatch.GetIndexList();

		// write number of vertex buffer
		uint32_t sizeHeader = (uint32_t) lineVertices.size();
		fwrite(&sizeHeader, sizeof(sizeHeader), 1, file);
		// write vertex buffer data
		fwrite(lineVertices.data(), sizeof(Vertex), lineVertices.size(), file);
		// write number of index buffer
		sizeHeader = (uint32_t)lineIndices.size();
		fwrite(&sizeHeader, sizeof(sizeHeader), 1, file);
		// write index buffer data
		fwrite(lineIndices.data(), sizeof(uint32_t), lineIndices.size(), file);
		// write scale matrix
		fwrite(glm::value_ptr(scene.scaleMatrix), sizeof(glm::mat4), 1, file);
		// write translation matrix
		fwrite(glm::value_ptr(scene.translationMatrix), sizeof(glm::mat4), 1, file);


		fclose(file);
		printf("Saved to file \"%s\"\n", filepath);
	}

	void SceneSerializer::Deserialize(Scene* scene, const char* filepath) {
		std::vector<Vertex>& lineVertices = scene->lineBatch.GetVertexList();
		std::vector<uint32_t>& lineIndices = scene->lineBatch.GetIndexList();


		printf("Load from file \"%s\"\n", filepath);

		FILE* file;
		fopen_s(&file, filepath, "rb");
		if (!file) {
			printf("Can not read from file \"%s\"\n", filepath);
			return;
		}
			
		Scene::CleanUp(*scene);

		uint32_t elemCount;
		// read number of vertices
		fread_s(&elemCount, sizeof(elemCount), sizeof(uint32_t), 1, file);
		// read vertex buffer
		lineVertices.resize(elemCount);
		fread_s(lineVertices.data(), elemCount * sizeof(Vertex), sizeof(Vertex), elemCount, file);

		// read number of indices
		fread_s(&elemCount, sizeof(elemCount), sizeof(uint32_t), 1, file);

		// read index buffer
		lineIndices.resize(elemCount);
		fread_s(lineIndices.data(), elemCount * sizeof(uint32_t), sizeof(uint32_t), elemCount, file);

		// read scale matrix
		fread_s(glm::value_ptr(scene->scaleMatrix), sizeof(scene->scaleMatrix), sizeof(scene->scaleMatrix), 1, file);

		// read translation matrix
		fread_s(glm::value_ptr(scene->translationMatrix), sizeof(scene->translationMatrix), sizeof(scene->translationMatrix), 1, file);


		fclose(file);
	}
}