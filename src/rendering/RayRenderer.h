#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Renderer.h"

class RayRenderer : public Renderer {
 public:
  // Constructor (inits shaders/shapes)
  RayRenderer(const Shader& shader);
  // Destructor
  ~RayRenderer() = default;
  // Renders a defined quad textured with given color
  void DrawRay(const std::vector<glm::vec2>& path, glm::vec4 color);

 private:
  // Initializes and configures the quad's buffer and vertex attributes
  void initRenderData();
};
