/*
 * LineRenderer.cpp
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

#include "LineRenderer.h"

LineRenderer::LineRenderer(const Shader& shader) : Renderer(shader) {
  this->initRenderData();
}

LineRenderer::~LineRenderer() {}

void LineRenderer::DrawLines(const std::vector<glm::vec2>& lines,
                             glm::vec4 color) {
  // Check if the path is empty
  if (lines.empty()) {
    return;
  }
  // Prepare transformations
  this->shader.Use();

  glBindVertexArray(this->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

  // Upload the path data to the GPU
  glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(glm::vec2), lines.data(),
               GL_STATIC_DRAW);
  // Set the vertex attribute pointers
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  //// Set line width
  // glLineWidth(2.0f); // Adjust the line width as needed

  // Draw the ray
  glDrawArrays(GL_LINES, 0, lines.size());

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  this->shader.SetVector4f("color", color);
}

void LineRenderer::initRenderData() {
  // Delete previous VAO and VBO if they exist
  glDeleteVertexArrays(1, &this->VAO);
  glDeleteBuffers(1, &this->VBO);
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
}
