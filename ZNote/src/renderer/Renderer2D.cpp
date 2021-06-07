#include "Renderer2D.h"
#include "Scene.h"
#include <GL/glew.h>
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <string.h>

#define MAX_TEXTURE_SLOTS (3)


struct ImageVertex {
	glm::vec2 position;
	glm::vec2 uv;
	uint8_t textureId;
};

struct GlMesh {
	uint32_t vao;
	uint32_t vbo;
	uint32_t ibo;
};

struct GlProgram {
	uint32_t id;
	uint32_t projectionMatrixLocation;
};

static struct {
	GlMesh lineMesh;
	GlProgram lineProgram;

	renderer::Batch<ImageVertex> imageBatch;
	GlMesh imageMesh;
	GlProgram imageProgram;

	glm::mat4 projectionMatrix;	

	uint32_t textureSlots[MAX_TEXTURE_SLOTS];
	
}renderData;

namespace app {

	static void DeleteMesh(GlMesh& mesh) {
		glDeleteVertexArrays(1, &mesh.vao);
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
	}

	static void InitializeEmptyMesh(GlMesh& mesh) {
		glGenVertexArrays(1, &mesh.vao);
		glBindVertexArray(mesh.vao);
		glGenBuffers(2, &mesh.vbo);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	}

	static void InitProgram(GlProgram& program) {
		glm::mat4 identiy = glm::mat4(1.0f);
		glUseProgram(program.id);
		program.projectionMatrixLocation = glGetUniformLocation(program.id, "viewProjectionMatrix");
		glUniformMatrix4fv(program.projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(identiy));
	}

	void Renderer2D::Initialize() {
		InitializeEmptyMesh(renderData.lineMesh);

		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // color

		// describe vertex layout for lines
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)sizeof(glm::vec2));

		renderData.lineProgram.id = utils::CreateLineShaderProgram();
		InitProgram(renderData.lineProgram);


		InitializeEmptyMesh(renderData.imageMesh);
		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // uv
		glEnableVertexAttribArray(2); // texture id

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImageVertex), NULL);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImageVertex), (const void*) sizeof(glm::vec2));
		glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(ImageVertex), (const void*) (sizeof(glm::vec2) * 2));

		renderData.imageProgram.id = utils::CreateImageShaderProgram();
		InitProgram(renderData.imageProgram);
		uint32_t shaderSamplers[MAX_TEXTURE_SLOTS];
		for (uint32_t i = 0; i < MAX_TEXTURE_SLOTS; i++) {
			shaderSamplers[i] = i;
		}
		uint32_t samplerSlotLoc = glGetUniformLocation(renderData.imageProgram.id, "u_textures");
		glUniform1iv(samplerSlotLoc, MAX_TEXTURE_SLOTS, (int*) shaderSamplers);
		
		memset(renderData.textureSlots, 0xFF, sizeof(renderData.textureSlots));
	}

	void Renderer2D::CleanUp() {
		glDeleteProgram(renderData.lineProgram.id);
		glDeleteProgram(renderData.imageProgram.id);
		DeleteMesh(renderData.lineMesh);
		DeleteMesh(renderData.imageMesh);
		
	}

	void Renderer2D::Begin(const glm::mat4& projection) {
		renderData.projectionMatrix = projection;
		renderData.imageBatch.Clear();
	}

	void Renderer2D::FlushImageBatch() {
		glUseProgram(renderData.imageProgram.id);
		// set uniform
		glUniformMatrix4fv(renderData.imageProgram.projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(renderData.projectionMatrix));

		glBindVertexArray(renderData.imageMesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, renderData.imageMesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.imageMesh.ibo);

		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)renderData.imageBatch.GetNumVertices() * renderData.imageBatch.GetVertexSize(), renderData.imageBatch.GetVertices(), GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)renderData.imageBatch.GetNumIndices() * sizeof(uint32_t), renderData.imageBatch.GetIndices(), GL_DYNAMIC_DRAW);

		glDrawElements(GL_TRIANGLES, (GLsizei)renderData.imageBatch.GetNumIndices(), GL_UNSIGNED_INT, NULL);
		// reset texture slots

		memset(renderData.textureSlots, 0xFF, sizeof(renderData.textureSlots));
		renderData.imageBatch.Clear();
	}

	static uint32_t FindTextureSlot(uint32_t id, bool* bind) {
		for (uint32_t i = 0; i < MAX_TEXTURE_SLOTS; i++) {
			if (renderData.textureSlots[i] == id) {
				*bind = false;
				return i;
			}else if (renderData.textureSlots[i] == 0xFFFFFFFF) {
				*bind = true;
				renderData.textureSlots[i] = id;
				return i;
			}
		}
		return 0xFFFFFFFF;
	}

	void Renderer2D::DrawImage(uint32_t textureId, const glm::vec2& position, const glm::vec2& size) {
		bool bind;
		uint32_t textureSlot = FindTextureSlot(textureId, &bind);
		if (textureSlot == 0xFFFFFFFF) {
			FlushImageBatch();
			textureSlot = FindTextureSlot(textureId, &bind);
		}
		if (bind) {
			glActiveTexture(GL_TEXTURE0 + textureSlot);
			glBindTexture(GL_TEXTURE_2D, textureId);
		}

		renderer::SubMesh<ImageVertex> mesh;
		ImageVertex imageVertex;

		imageVertex.position = position;
		imageVertex.uv = glm::vec2(0.0f, 0.0f);
		imageVertex.textureId = textureSlot;
		mesh.Vertex(imageVertex);

		imageVertex.position.x += size.x;
		imageVertex.uv.x = 1.0f;
		mesh.Vertex(imageVertex);

		imageVertex.position.y += size.y;
		imageVertex.uv.y = 1.0f;
		mesh.Vertex(imageVertex);

		imageVertex.position.x -= size.x;
		imageVertex.uv.x = 0.0f;
		mesh.Vertex(imageVertex);

		mesh.Index(0);
		mesh.Index(1);
		mesh.Index(2);

		mesh.Index(0);
		mesh.Index(2);
		mesh.Index(3);

		renderData.imageBatch.Insert(mesh);
	}


	template<>
	void Renderer2D::DrawBatch(const renderer::Batch<Vertex>& lineBatch) {
		glUseProgram(renderData.lineProgram.id);
		// set uniform
		glUniformMatrix4fv(renderData.lineProgram.projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(renderData.projectionMatrix));

		glBindVertexArray(renderData.lineMesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, renderData.lineMesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.lineMesh.ibo);

		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) lineBatch.GetNumVertices() * lineBatch.GetVertexSize(), lineBatch.GetVertices(), GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) lineBatch.GetNumIndices() * sizeof(uint32_t), lineBatch.GetIndices(), GL_DYNAMIC_DRAW);		

		glDrawElements(GL_LINES, (GLsizei)lineBatch.GetNumIndices(), GL_UNSIGNED_INT, NULL);
	}

	void Renderer2D::End() {
		FlushImageBatch();
	}
}