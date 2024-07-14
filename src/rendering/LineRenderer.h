#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Renderer.h"

class LineRenderer : public Renderer {
 public:
  // Constructor (inits shaders/shapes)
  LineRenderer(const Shader& shader);
  // Destructor
  ~LineRenderer();
  // Renders Lines
  void DrawLines(const std::vector<glm::vec2>& lines, glm::vec4 color);

 private:
  // Initializes and configures buffer and vertex attributes
  void initRenderData();
};