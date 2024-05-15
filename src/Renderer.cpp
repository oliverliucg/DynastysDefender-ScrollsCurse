#include "Renderer.h"


SizePadding Renderer::actual_window_size_padding_ = SizePadding();

SizePadding Renderer::expected_window_size_padding_ = SizePadding();

Renderer::Renderer(const Shader& shader) : shader(shader) {}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Renderer::SetExpectedWindowSizePadding(SizePadding sizePadding) {
	expected_window_size_padding_ = sizePadding;
}

void Renderer::SetActualWindowSizePadding(SizePadding sizePadding) {
	actual_window_size_padding_ = sizePadding;
}

SizePadding Renderer::GetExpectedWindowSizePadding() {
	return expected_window_size_padding_;
}

SizePadding Renderer::GetActualWindowSizePadding() {
	return actual_window_size_padding_;
}
