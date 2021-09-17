#include "SvgSerializer.h"
#include <limits>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <core/Logger.h>


namespace app {

	struct WorldViewport {
		float x;
		float y;
		float width;
		float height;
	};

	static void WriteString(FILE* file, const char* string) {
		fwrite(string, strlen(string), 1, file);
	}

	static uint32_t WriteStroke(FILE* file, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t beginIdx, const WorldViewport& viewport) {
		uint32_t newOffset = (uint32_t) indices.size();
		
		char headerBuffer[100];
		Color color = vertices[indices[beginIdx]].color;
		int len = sprintf_s(headerBuffer, sizeof(headerBuffer), "\t<path stroke-width=\"%dpx\" stroke=\"rgb(%d, %d, %d)\" fill=\"none\" d=\"", 1, color.r, color.g, color.b);
		fwrite(headerBuffer, len, 1, file);

		const float scale = 1.0f / std::max(viewport.width, viewport.height);

		for (uint32_t i = beginIdx; i < (uint32_t) indices.size(); i++) {
			uint32_t idx = indices[i];
			ASSERT(idx < vertices.size());
			const Vertex& vertex = vertices[idx];

			if (i == beginIdx) {
				WriteString(file, "M ");
			}else {
				WriteString(file, "L ");
			}

			glm::vec2 pos = vertex.position;
			pos -= glm::vec2(viewport.x, viewport.y);
			pos.x *= scale;
			pos.y *= scale;
			pos.x *= 500;
			pos.y *= 500;
			pos.y = 500 - pos.y;

			// write position
			char buffer[30] = { 0 };
			int len = sprintf_s(buffer, sizeof(buffer), "%f,%f ", pos.x, pos.y);
			fwrite(buffer, len, 1, file);

			if (i + 1 == indices.size() || (indices[i + 1] != idx && i & 1)) {
				newOffset = i + 1;
				break;
			}
		}



		WriteString(file, "\"/>\n");
		return newOffset;
	}

	void SvgSerializer::Serialize(const char* filepath, const Scene& scene) {
		// find min and max x and y position
		float maxX = -std::numeric_limits<float>::max();
		float minX = std::numeric_limits<float>::max();
		float maxY = -std::numeric_limits<float>::max();
		float minY = std::numeric_limits<float>::max();

		const std::vector<Vertex>& vertices = scene.lineBatch.GetVertexList();
		const std::vector<uint32_t>& indices = scene.lineBatch.GetIndexList();

		for (const Vertex& vertex : vertices) {
			maxX = std::max(maxX, vertex.position.x);
			maxY = std::max(maxY, vertex.position.y);
			minX = std::min(minX, vertex.position.x);
			minY = std::min(minY, vertex.position.y);
		}

		WorldViewport viewport;
		viewport.x = minX;
		viewport.y = minY;
		viewport.width = maxX - minX;
		viewport.height = maxY - minY;

		FILE* file;
		fopen_s(&file, filepath, "w");
		if (!file)
			return;

		// <header>
		WriteString(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		WriteString(file, "<svg xmlns=\"http://www.w3.org/2000/svg\"\n");
		WriteString(file, "\txmlns:xlink=\"http://www.w3.org/1999/xlink\"\n");
		WriteString(file, "\tversion = \"1.1\" baseProfile = \"full\"\n");
		WriteString(file, "\tstyle=\"background: rgb(25, 25, 25)\"\n");

		char lineBuffer[1024] = { 0 };
		int len = sprintf_s(lineBuffer, sizeof(lineBuffer), "\twidth=\"%fpx\" height=\"%fpx\"\n", 500.0f, 500.0f);
		fwrite(lineBuffer, len, 1, file);

		len = sprintf_s(lineBuffer, sizeof(lineBuffer), "\tviewbox=\"%f %f %f %f\">\n\n", 0.0f, 0.0f, maxX - minX, maxY - minY);
		fwrite(lineBuffer, len, 1, file);
		// </header>

		WriteString(file, "\t<title>ZNote-Export</title>\n\n");


		// path https://de.wikipedia.org/wiki/Scalable_Vector_Graphics#Pfad

		uint32_t strokeBeginIdx = 0;

		while (strokeBeginIdx != indices.size()) {
			strokeBeginIdx = WriteStroke(file, vertices, indices, strokeBeginIdx, viewport);
		}


		WriteString(file, "</svg>");
		fclose(file);
	}
}