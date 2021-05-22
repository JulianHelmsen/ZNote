#include "SceneSerializer.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include "core/Logger.h"
#include "renderer/TextureLoader.h"
#include "Compression.h"
#include <cstddef>


#define ERROR_CRITICAL (1 << 30)
#define ERROR_UNSUPPORTED_TYPE (1)
#define ERROR_NOT_ENOUGH_MEMORY (2 | ERROR_CRITICAL)
#define ERROR_SUCCESS (0)


namespace app {

	struct SaveFileHeader {
		Compression::CompressionAlgorithm compressionAlgorithm;
	};

	enum class SerializationTypes : uint32_t {
		VERTEX_BUFFER = 0,
		INDEX_BUFFER = 1,
		SCALE_MATRIX = 2,
		TRANSLATION_MATRIX = 3,
		IMAGE_EXTERNAL = 4,
		IMAGE_INCLUDED = 5,
		STROKE_LENGTH = 6
	};

	static std::string ToErrorString(uint32_t errorcode) {
#define TO_STRING(code) if(errorcode == code) return #code;
		TO_STRING(ERROR_UNSUPPORTED_TYPE);
		TO_STRING(ERROR_NOT_ENOUGH_MEMORY);
		return "ERROR_SUCCESS";
	}

	template<typename T>
	T& Read(uint8_t** position) {
		T* arr = (T*)*position;
		*position += sizeof(T);
		return *arr;
	}

	static void Copy(void* dest, uint8_t** data, uint32_t bytes) {
		memcpy(dest, *data, bytes);
		*data += bytes;
	}

	template<typename T>
	void Write(std::vector<uint8_t>& buffer, const T* data, size_t count) {
		size_t bytes = sizeof(T) * count;
		uint8_t* values = (uint8_t*) data;
		for (size_t i = 0; i < bytes; i++)
			buffer.push_back(values[i]);
	}

	static void SerializeVertexBuffer(std::vector<uint8_t>& writeBuffer, SerializationTypes type, const std::vector<Vertex>& lineVertices) {
		uint32_t sizeHeader = (uint32_t) lineVertices.size();
		Write(writeBuffer, &type, 1);
		Write(writeBuffer, &sizeHeader, 1);
		Write(writeBuffer, lineVertices.data(), lineVertices.size());
	}

	static void SerializeIndexBuffer(std::vector<uint8_t>& writeBuffer, SerializationTypes type, const std::vector<uint32_t>& indices) {
		uint32_t sizeHeader = (uint32_t)indices.size();
		Write(writeBuffer, &type, 1);
		Write(writeBuffer, &sizeHeader, 1);
		Write(writeBuffer, indices.data(), indices.size());
	}

	static void SerializeMatrix(std::vector<uint8_t>& writeBuffer, SerializationTypes type, const glm::mat4& matrix) {
		Write(writeBuffer, &type, 1);
		Write(writeBuffer, glm::value_ptr(matrix), 16);
	}

	static void SerializeExternalImage(std::vector<uint8_t>& writeBuffer, SerializationTypes type, const Image& image) {

		uint32_t filepathLen = (uint32_t)image.filepath.size();
		Write(writeBuffer, &type, 1);
		Write(writeBuffer, &filepathLen, 1);
		Write(writeBuffer, image.filepath.c_str(), filepathLen);
		Write(writeBuffer, &image.centerPos, 1);
		Write(writeBuffer, &image.size, 1);
	}
	
	static void SerializeStrokeLengths(std::vector<uint8_t>& writeBuffer, SerializationTypes type, const std::vector<uint32_t>& indices) {
		uint32_t currentLen = 0;
		std::vector<uint32_t> lengths;
		lengths.reserve(500);

		for (uint32_t i = 1; i < indices.size() - 1; i += 2) {
			if (indices[i] == indices[i + 1]) {
				currentLen++;
			}else {
				lengths.push_back(currentLen + 1);
				currentLen = 0;
			}
		}
		lengths.push_back(currentLen + 1);
		uint32_t numStrokes = (uint32_t) lengths.size();

		Write(writeBuffer, &type, 1);
		Write(writeBuffer, &numStrokes, 1);
		for (uint32_t val : lengths) {
			Write(writeBuffer, &val, 1);
		}
			
	}


	void SceneSerializer::Serialize(const char* filepath, const Scene& scene) {
		const std::vector<Vertex>& lineVertices = scene.lineBatch.GetVertexList();
		const std::vector<uint32_t>& lineIndices = scene.lineBatch.GetIndexList();

		std::vector<uint8_t> writeBuffer;
		
		SerializeVertexBuffer(writeBuffer, SerializationTypes::VERTEX_BUFFER, lineVertices);
		SerializeStrokeLengths(writeBuffer, SerializationTypes::STROKE_LENGTH, lineIndices);


		// write scale matrix
		SerializeMatrix(writeBuffer, SerializationTypes::SCALE_MATRIX, scene.scaleMatrix);
		SerializeMatrix(writeBuffer, SerializationTypes::TRANSLATION_MATRIX, scene.translationMatrix);

		
		for (const Image& image : scene.images) {
			// write filepath
			if (image.filepath == "(none)") {
				// TODO: save image as reference
			}else {
				SerializeExternalImage(writeBuffer, SerializationTypes::IMAGE_EXTERNAL, image);
			}
		}

		// compress

		FILE* file;
		fopen_s(&file, filepath, "wb");


		// Create the file header
		uint32_t headerSize = sizeof(SaveFileHeader);
		SaveFileHeader header;
		header.compressionAlgorithm = Compression::CompressionAlgorithm::NONE;

		// write the file header
		fwrite(&headerSize, 1, sizeof(uint32_t), file);
		fwrite(&header, 1, sizeof(SaveFileHeader), file);

		// write the files content
		fwrite(writeBuffer.data(), writeBuffer.size(), 1, file);

		fclose(file);
		printf("Saved to file \"%s\"\n", filepath);
	}

	

	static uint32_t ReadVertexBuffer(std::vector<Vertex>& lineVertices, uint8_t** position, uint8_t* end) {
		if (*position + sizeof(uint32_t) > end)
			return ERROR_NOT_ENOUGH_MEMORY;
		uint32_t size = Read<uint32_t>(position);
		if (*position + sizeof(Vertex) * size > end)
			return ERROR_NOT_ENOUGH_MEMORY;
		lineVertices.resize(size);
		Copy(lineVertices.data(), position, size * sizeof(Vertex));
		return ERROR_SUCCESS;
	}

	static uint32_t ReadIndexBuffer(std::vector<uint32_t>& indices, uint8_t** position, uint8_t* end) {
		if (*position + sizeof(uint32_t) > end)
			return ERROR_NOT_ENOUGH_MEMORY;
		uint32_t size = Read<uint32_t>(position);
		if (*position + sizeof(uint32_t) * size > end)
			return ERROR_NOT_ENOUGH_MEMORY;
		indices.resize(size);
		Copy(indices.data(), position, size * sizeof(uint32_t));
		return ERROR_SUCCESS;
	}

	static uint32_t ReadMatrix(glm::mat4& dest, uint8_t** position, uint8_t* end) {
		if (*position + sizeof(float) * 16 > end)
			return ERROR_NOT_ENOUGH_MEMORY;
		Copy(glm::value_ptr(dest), position, sizeof(float) * 16);
		return ERROR_SUCCESS;
	}


	static uint32_t ReadExternalImage(Scene& scene, uint8_t** position, uint8_t* end) {
		if (*position + sizeof(uint32_t) > end)
			return ERROR_NOT_ENOUGH_MEMORY;

		uint32_t filepathLen = Read<uint32_t>(position);
		if (*position + sizeof(filepathLen) > end)
			return ERROR_NOT_ENOUGH_MEMORY;

		Image image;
		image.filepath.resize(filepathLen);
		Copy(image.filepath.data(), position, filepathLen);

		image.textureId = utils::TextureLoader::LoadTexture(image.filepath.c_str());
		
		if (*position + sizeof(glm::vec2) > end)
			return ERROR_NOT_ENOUGH_MEMORY;
		image.centerPos = Read<glm::vec2>(position);
		if (*position + sizeof(glm::vec2) > end)
			return ERROR_NOT_ENOUGH_MEMORY;
		image.size = Read<glm::vec2>(position);
		scene.images.push_back(image);
		return ERROR_SUCCESS;
	}

	static uint32_t ReadStrokeLengths(Scene& scene, uint8_t** position, uint8_t* end) {
		if (*position + sizeof(uint32_t) > end)
			return ERROR_NOT_ENOUGH_MEMORY;

		uint32_t numStrokes = Read<uint32_t>(position);
		
		if (*position + sizeof(uint32_t) * numStrokes > end)
			return ERROR_NOT_ENOUGH_MEMORY;
		
		uint32_t* strokeList = (uint32_t*) *position;
		*position += sizeof(uint32_t) * numStrokes; // skip

		std::vector<uint32_t>& indices = scene.lineBatch.GetIndexList();
		indices.clear();
		uint32_t idx = 0;
		for (uint32_t i = 0; i < numStrokes; i++) {
			for (uint32_t j = 0; j < strokeList[i]; j++) {
				indices.push_back(idx);
				indices.push_back(idx + 1);
				idx++;
			}
			idx++;
		}


		return ERROR_SUCCESS;
	}

	static uint32_t DeserializeComponent(Scene& scene, uint8_t** position, uint8_t* end) {
		SerializationTypes type = Read<SerializationTypes>(position);
		switch (type) {
		case SerializationTypes::VERTEX_BUFFER:
			return ReadVertexBuffer(scene.lineBatch.GetVertexList(), position, end);
		case SerializationTypes::INDEX_BUFFER:
			return ReadIndexBuffer(scene.lineBatch.GetIndexList(), position, end);
		case SerializationTypes::SCALE_MATRIX:
			return ReadMatrix(scene.scaleMatrix, position, end);
		case SerializationTypes::TRANSLATION_MATRIX:
			return ReadMatrix(scene.translationMatrix, position, end);
		case SerializationTypes::IMAGE_EXTERNAL:
			return ReadExternalImage(scene, position, end);
		case SerializationTypes::STROKE_LENGTH:
			return ReadStrokeLengths(scene, position, end);
		}
		return ERROR_UNSUPPORTED_TYPE;
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

		uint32_t fileSize;
		uint8_t* fileContent;
		uint8_t* fileContentBase;
		fseek(file, 0, SEEK_END);
		fileSize = (uint32_t) ftell(file);
		rewind(file);
		fileContent = new uint8_t[fileSize];
		fileContentBase = fileContent;
		fread_s(fileContent, fileSize, 1, fileSize, file);
		fclose(file);

		Scene::CleanUp(*scene);

		// Parse uncompressed header
		uint32_t headerSize = Read<uint32_t>(&fileContent);
		if (headerSize > fileSize) {
			LOG("file is corrupted!\n");
			delete[] fileContent;
			return;
		}

		SaveFileHeader* header = (SaveFileHeader*) fileContent;
		fileContent += headerSize;
		uint32_t contentSize = fileSize - sizeof(headerSize) - headerSize;
		
		if (offsetof(SaveFileHeader, compressionAlgorithm) < headerSize) {
			// uncompress here
			Compression decompression{ header->compressionAlgorithm };
			
			
		}


		


		uint8_t* position = fileContent;
		uint8_t* end = fileContentBase + fileSize;
		while (position < end) {
			uint32_t errorcode = DeserializeComponent(*scene, &position, end);
			if (errorcode & ERROR_NOT_ENOUGH_MEMORY) {
				LOG("criticalError: %s\n", ToErrorString(errorcode).c_str());
				break;
			}
#ifdef DEBUG
			else if(errorcode)
				LOG("warning: %s\n", ToErrorString(errorcode).c_str());
#endif
		}
			
		delete[] fileContentBase;
	}
}