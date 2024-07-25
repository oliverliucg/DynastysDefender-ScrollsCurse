#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Renderer.h"
#include "ResourceManager.h"

class CircleRenderer : public Renderer {
 public:
  // Constructor (inits shaders/shapes)
  CircleRenderer(const Shader& shader, float percentageOfCircle);
  // Destructor
  ~CircleRenderer() = default;
  // Renders a circle / triangle fan
  void DrawCircle(glm::vec2 position, float radius, float rotate = 0.0f,
                  glm::vec4 color = glm::vec4(1.0f));
  // Renders the edge of a circle
  void DrawCircleEdge(glm::vec2 position, float radius, float rotate = 0.0f,
                      glm::vec4 color = glm::vec4(1.0f));

 private:
  // percentage of circle
  float percentageOfCircle{1.f};
  // number of segments
  size_t numSegments{0};
  // Initializes and configures the quad's buffer and vertex attributes
  void initRenderData(float percentageOfCircle);
};
