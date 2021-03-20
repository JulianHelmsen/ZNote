#include "Renderer2D.h"
#include "Scene.h"
#include <GL/glew.h>
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

struct GlMesh {
	uint32_t vao;
	uint32_t vbo;
	uint32_t ibo;
};

struct GLProgram {
	uint32_t id;
	uint32_t projectionMatrixLocation;
};

static struct {
	GlMesh lineMesh;
	GLProgram lineProgram;



	glm::mat4 projectionMatrix;
	
	
}renderData;

namespace app {

	static void DeleteMesh(GlMesh& mesh) {
		glDeleteVertexArrays(1, &mesh.vao);
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
	}

	void Renderer2D::Initialize() {
		glm::mat4 identiy = glm::mat4(1.0f);
		glGenVertexArrays(1, &renderData.lineMesh.vao);
		glBindVertexArray(renderData.lineMesh.vao);
		glGenBuffers(2, &renderData.lineMesh.vbo);
		
		glBindBuffer(GL_ARRAY_BUFFER, renderData.lineMesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.lineMesh.ibo);

		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // color

		// describe vertex layout for lines
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)sizeof(glm::vec2));

		renderData.lineProgram.id = utils::CreateLineShaderProgram();
		glUseProgram(renderData.lineProgram.id);
		renderData.lineProgram.projectionMatrixLocation = glGetUniformLocation(renderData.lineProgram.id, "viewProjectionMatrix");	
		glUniformMatrix4fv(renderData.lineProgram.projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(identiy));

	}

	void Renderer2D::CleanUp() {
		glDeleteProgram(renderData.lineProgram.id);
		DeleteMesh(renderData.lineMesh);
		
	}

	void Renderer2D::Begin(const glm::mat4& projection) {
		renderData.projectionMatrix = projection;
	}


	void Renderer2D::DrawImage(uint32_t rendererId, glm::vec2& position, glm::vec2& size) {

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

	}
}