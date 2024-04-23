#include "ColorRenderer.h"

ColorRenderer::ColorRenderer(const Shader& shader): Renderer(shader)
{
	this->initRenderData();
}

ColorRenderer::~ColorRenderer() {}

void ColorRenderer::DrawColor(glm::vec2 position, glm::vec2 size, float rotate, glm::vec2 rotationPivot, glm::vec4 color)
{	

	// Prepare transformations
	this->shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));

	// First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	// Move origin of rotation to center of quad
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));

	// Then rotate
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));

	// Move origin back
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	// Last scale
	model = glm::scale(model, glm::vec3(size, 1.0f));

	this->shader.SetMatrix4("model", model);
	this->shader.SetVector4f("color", color);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void ColorRenderer::DrawEdge(glm::vec2 position, glm::vec2 size, float rotate, glm::vec2 rotationPivot, glm::vec4 color, bool hideVerticalEdge, bool hideHorizontalEdge) {
	// Prepare transformations
	this->shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));

	// First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	// Move origin of rotation to center of quad
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));

	// Then rotate
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));

	// Move origin back
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	// Last scale
	model = glm::scale(model, glm::vec3(size, 1.0f));

	this->shader.SetMatrix4("model", model);
	this->shader.SetVector4f("color", color);
	glBindVertexArray(this->VAO);
	assert((!hideHorizontalEdge || !hideVerticalEdge) && "one pair of edges should be visible");
	if (hideVerticalEdge && !hideHorizontalEdge) {
		glDrawArrays(GL_LINES, 1, 4);
	}
	else if (!hideVerticalEdge && hideHorizontalEdge) {
		glDrawArrays(GL_LINES, 2, 4);
	}
	else
	{
		glDrawArrays(GL_LINE_LOOP, 1, 4);
	}
	glBindVertexArray(0);
}

void ColorRenderer::initRenderData()
{
	// Configure VAO/VBO
	float vertices[] = {
		// positions   
		0.0f, 1.0f, 0.0f,   // top left
		1.0f, 0.0f, 0.0f,   // bottom right
		0.0f, 0.0f, 0.0f,   // bottom left

		0.0f, 1.0f, 0.0f,   // top left
		1.0f, 1.0f, 0.0f,   // top right
		1.0f, 0.0f, 0.0f    // bottom right
	};

	glGenVertexArrays(1, &(this->VAO));
	glGenBuffers(1, &(this->VBO));

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//// color attribute
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

