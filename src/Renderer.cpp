#include "Renderer.h"

float Renderer::scaleRendering = 1.0f;

Renderer::Renderer(const Shader& shader) : shader(shader) {}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Renderer::SetScaleRendering(float scale) {
    Renderer::scaleRendering = scale;
}