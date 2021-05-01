#pragma once

#include <vector>
#include <stdint.h>

namespace renderer {

	template<typename T>
	class Batch;

	template<typename VertexType>
	class SubMesh {
	private:
		VertexType vertices[4];
		uint32_t numVertices = 0;
		uint32_t indices[6];
		uint32_t numIndices = 0;
	public:

		SubMesh& Vertex(const VertexType& vertex) {
			vertices[numVertices++] = vertex;
			return *this;
		}
		SubMesh& Index(uint32_t idx) {
			indices[numIndices++] = idx;
			return *this;
		}

		friend Batch<VertexType>;
	};

	template<typename VertexType>
	class Batch {
	public:
		void Insert(const SubMesh<VertexType>& submesh) {
			uint32_t baseIdx = GetNumVertices();
			for (uint32_t i = 0; i < submesh.numVertices; i++)
				InsertVertex(submesh.vertices[i]);

			for(uint32_t i = 0; i < submesh.numIndices; i++)
				InsertIndex(submesh.indices[i] + baseIdx);
		}

		void InsertVertex(const VertexType& vertex) {
			vertices.push_back(vertex);
		}
		void InsertIndex(const uint32_t index) {
			indices.push_back(index);
		}

		inline const VertexType* GetVertices() const { return vertices.data(); }
		inline VertexType* GetVertices() { return vertices.data(); }
		constexpr uint32_t GetVertexSize() const { return sizeof(VertexType); }
		inline const uint32_t* GetIndices() const { return indices.data(); }
		inline uint32_t* GetIndices() { return indices.data(); }

		uint32_t GetNumIndices() const { return (uint32_t) indices.size(); }
		uint32_t GetNumVertices() const { return (uint32_t) vertices.size(); }

		const std::vector<VertexType>& GetVertexList() const { return vertices; };
		const std::vector<uint32_t>& GetIndexList() const { return indices; };
		std::vector<VertexType>& GetVertexList() { return vertices; };
		std::vector<uint32_t>& GetIndexList() { return indices; };
		void Clear() { vertices.clear(); indices.clear(); }
		
	private:
		mutable std::vector<VertexType> vertices;
		mutable std::vector<uint32_t> indices;
	};
}