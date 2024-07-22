/*
 * PartialTextureRenderer.cpp
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

#include "PartialTextureRenderer.h"

PartialTextureRenderer::PartialTextureRenderer(const Shader& shader)
    : Renderer(shader) {
  this->initRenderData();
}

PartialTextureRenderer::~PartialTextureRenderer() {}

void PartialTextureRenderer::DrawPartialTexture(
    const Texture2D& texture, glm::vec2 position, glm::vec2 size,
    glm::vec2 redChannelRange, glm::vec2 greenChannelRange,
    glm::vec2 blueChannelRange, glm::vec2 alphaChannelRange, float rotate,
    glm::vec2 rotationPivot) {
  // Prepare transformations
  this->shader.Use();
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position, 0.0f));

  // First translate (transformations are: scale happens first, then rotation
  // and then finall translation happens; reversed order)

  // Move origin of rotation to center of quad
  model = glm::translate(model, glm::vec3(rotationPivot.x * size.x,
                                          rotationPivot.y * size.y, 0.0f));

  // Rotate
  model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));

  // Move to origin
  model = glm::translate(model, glm::vec3(-rotationPivot.x * size.x,
                                          -rotationPivot.y * size.y, 0.0f));

  // Last scale
  model = glm::scale(model, glm::vec3(size, 1.0f));

  this->shader.SetMatrix4("model", model);
  this->shader.SetVector2f("redChannelRange", redChannelRange);
  this->shader.SetVector2f("greenChannelRange", greenChannelRange);
  this->shader.SetVector2f("blueChannelRange", blueChannelRange);
  this->shader.SetVector2f("alphaChannelRange", alphaChannelRange);

  glActiveTexture(GL_TEXTURE0);
  texture.Bind();

  glBindVertexArray(this->VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void PartialTextureRenderer::initRenderData() {
  // Configure VAO/VBO

  float vertices[] = {
      // positions   // texture coords
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // top right
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // top left

      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom right
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f   // top right
  };

  // Delete previous VAO and VBO if they exist
  glDeleteVertexArrays(1, &this->VAO);
  glDeleteBuffers(1, &this->VBO);

  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);

  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(this->VAO);

  // glEnableVertexAttribArray(0);
  // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
  // (void*)0);
  //  position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // texture coords attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
