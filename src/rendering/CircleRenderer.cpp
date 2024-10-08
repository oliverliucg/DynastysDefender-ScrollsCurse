/*
 * CircleRenderer.cpp
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

#include "CircleRenderer.h"

CircleRenderer::CircleRenderer(const Shader& shader, float percentageOfCircle)
    : percentageOfCircle(percentageOfCircle),
      numSegments(static_cast<size_t>(360.f * percentageOfCircle)),
      Renderer(shader) {
  this->initRenderData(percentageOfCircle);
}

void CircleRenderer::DrawCircle(glm::vec2 position, float radius, float rotate,
                                glm::vec4 color) {
  // Prepare transformations
  this->shader.Use();
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position, 0.0f));

  // Rotate
  model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));

  // Scale
  model = glm::scale(model, glm::vec3(radius, radius, 1.0f));

  this->shader.SetMatrix4("model", model);
  this->shader.SetVector4f("color", color);
  glBindVertexArray(this->VAO);
  glDrawArrays(GL_TRIANGLE_FAN, 0,
               numSegments + 2);  // +2 for center and closing
  glBindVertexArray(0);
}

void CircleRenderer::DrawCircleEdge(glm::vec2 position, float radius,
                                    float rotate, glm::vec4 color) {
  // Prepare transformations
  this->shader.Use();
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position, 0.0f));

  // Rotate
  model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));

  // Scale
  model = glm::scale(model, glm::vec3(radius, radius, 1.0f));

  this->shader.SetMatrix4("model", model);
  this->shader.SetVector4f("color", color);
  glBindVertexArray(this->VAO);
  glDrawArrays(GL_LINE_STRIP, 1, numSegments + 1);
  glBindVertexArray(0);
}

void CircleRenderer::initRenderData(float percentageOfCircle) {
  // Construct the vertices of the circle using the given angle
  std::vector<float> vertices;
  // Add center vertex
  vertices.emplace_back(0.0f);  // x
  vertices.emplace_back(0.0f);  // y
  vertices.emplace_back(0.0f);  // z
  // total angle of the circle
  float totalAngle = percentageOfCircle * 2.0f * glm::pi<float>();

  for (size_t i = 0; i <= numSegments; ++i) {
    float angle = totalAngle * i / numSegments;
    vertices.emplace_back(glm::cos(angle));  // x
    vertices.emplace_back(glm::sin(angle));  // y
    vertices.emplace_back(0.0f);             // z
  }

  // Delete previous VAO and VBO if they exist
  glDeleteVertexArrays(1, &this->VAO);
  glDeleteBuffers(1, &this->VBO);

  // Configure VAO/VBO
  glGenVertexArrays(1, &(this->VAO));
  glGenBuffers(1, &(this->VBO));

  glBindVertexArray(this->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  //// color attribute
  // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
  // (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
