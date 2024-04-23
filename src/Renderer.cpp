#include "Renderer.h"

Renderer::Renderer(const Shader& shader) : shader(shader) {}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
