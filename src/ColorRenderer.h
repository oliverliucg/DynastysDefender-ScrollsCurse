#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"

class ColorRenderer : public Renderer {
 public:
  // Constructor (inits shaders/shapes)
  ColorRenderer(const Shader& shader);
  // Destructor
  ~ColorRenderer();
  // Renders a defined quad with given color
  void DrawColor(glm::vec2 position, glm::vec2 size = glm::vec2(100.0f, 100.0f),
                 float rotate = 0.0f,
                 glm::vec2 rotationPivot = glm::vec2(0.5f, 0.5f),
                 glm::vec4 color = glm::vec4(1.0f));
  // Renders the edge of a defined quad with given color
  void DrawEdge(glm::vec2 position, glm::vec2 size = glm::vec2(100.0f, 100.0f),
                float rotate = 0.0f,
                glm::vec2 rotationPivot = glm::vec2(0.5f, 0.5f),
                glm::vec4 color = glm::vec4(1.0f),
                bool hideVerticalEdge = false, bool hideHorizontalEdge = false);

 private:
  // Initializes and configures the quad's buffer and vertex attributes
  void initRenderData();
};
