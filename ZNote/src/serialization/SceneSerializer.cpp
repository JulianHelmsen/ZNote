#include "SceneSerializer.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include "renderer/TextureLoader.h"


namespace app {

	template<typename T>
	void Write(std::vector<uint8_t>& buffer, const T* data, size_t count) {
		size_t bytes = sizeof(T) * count;
		uint8_t* values = (uint8_t*) data;
		for (size_t i = 0; i < bytes; i++)
			buffer.push_back(values[i]);
	}

	template<typename T>
	static void Serialize(std::vector<uint8_t>& writeBuffer, uint32_t blockId, const T& data) {
		Write(writeBuffer, &blockId, 1);

	}

	void SceneSerializer::Serialize(const char* filepath, const Scene& scene) {
		const std::vector<Vertex>& lineVertices = scene.lineBatch.GetVertexList();
		const std::vector<uint32_t>& lineIndices = scene.lineBatch.GetIndexList();

		std::vector<uint8_t> writeBuffer;
		
		// write number of vertex buffer
		uint32_t sizeHeader = (uint32_t) lineVertices.size();
		Write(writeBuffer, &sizeHeader, 1);
		// write vertex buffer data
		Write(writeBuffer, lineVertices.data(), lineVertices.size());
		// write number of index buffer
		sizeHeader = (uint32_t)lineIndices.size();
		Write(writeBuffer, &sizeHeader, 1);
		// write index buffer data
		Write(writeBuffer, lineIndices.data(), lineIndices.size());
		// write scale matrix
		Write(writeBuffer, glm::value_ptr(scene.scaleMatrix), 16);
		// write translation matrix
		Write(writeBuffer, glm::value_ptr(scene.translationMatrix), 16);

		// serialize images
		uint32_t numImages = (uint32_t) scene.images.size();
		Write(writeBuffer, &numImages, 1);

		for (const Image& image : scene.images) {
			// write filepath
			if (image.filepath == "(none)") {
				// TODO: save image as reference
				
			}else {
				uint32_t filepathLen = (uint32_t)image.filepath.size();
				Write(writeBuffer, &filepathLen, 1);
				Write(writeBuffer, image.filepath.c_str(), filepathLen);
				Write(writeBuffer, &image.centerPos, 1);
				Write(writeBuffer, &image.size, 1);
			}
		}

		FILE* file;
		fopen_s(&file, filepath, "wb");
		fwrite(writeBuffer.data(), writeBuffer.size(), 1, file);
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

		uint32_t numTextures;
		fread_s(&numTextures, sizeof(uint32_t), sizeof(uint32_t), 1, file);
		scene->images.reserve(numTextures);
		for (uint32_t i = 0; i < numTextures; i++) {
			uint32_t filepathLen;
			fread_s(&filepathLen, sizeof(filepathLen), sizeof(filepathLen), 1, file);

			Image image;
			image.filepath.resize(filepathLen);
			fread_s((char*) image.filepath.c_str(), filepathLen, sizeof(char), filepathLen, file);
			image.textureId = utils::TextureLoader::LoadTexture(image.filepath.c_str());
			fread_s(&image.centerPos, sizeof(glm::vec2), sizeof(glm::vec2), 1, file);
			fread_s(&image.size, sizeof(glm::vec2), sizeof(glm::vec2), 1, file);
			scene->images.push_back(image);
		}

		fclose(file);
	}
}