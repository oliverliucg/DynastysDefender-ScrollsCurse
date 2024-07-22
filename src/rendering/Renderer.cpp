/*
 * Renderer.cpp
 * Copyright (C) 2024 Oliver Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
