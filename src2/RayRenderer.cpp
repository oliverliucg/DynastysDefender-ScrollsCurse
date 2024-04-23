#include "RayRenderer.h"

RayRenderer::RayRenderer(const Shader& shader) : Renderer(shader)
{
	this->initRenderData();
}

RayRenderer::~RayRenderer() {}

void RayRenderer::DrawRay(const std::vector<glm::vec2>& path, glm::vec4 color)
{

	// Check if the path is empty
	if (path.empty()) {
		return;
	}
	// Prepare transformations
	this->shader.Use();
	
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	// Upload the path data to the GPU
	glBufferData(GL_ARRAY_BUFFER, path.size() * sizeof(glm::vec2), path.data(), GL_DYNAMIC_DRAW);
	// Set the vertex attribute pointers
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//// Set line width
	//glLineWidth(2.0f); // Adjust the line width as needed

	// Draw the ray
    glDrawArrays(GL_LINE_STRIP, 0, path.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->shader.SetVector4f("color", color);
}

void RayRenderer::initRenderData()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
}

