#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "ResourceManager.h"

class CircleRenderer : public Renderer
{
public:
    // Constructor (inits shaders/shapes)
    CircleRenderer(const Shader& shader, float percentageOfCircle);
    // Destructor
    ~CircleRenderer();
    // Renders a circle / triangle fan
    void DrawCircle(glm::vec2 position, float radius, float rotate = 0.0f, glm::vec4 color = glm::vec4(1.0f));
    // Renders the edge of a circle
    void DrawCircleEdge(glm::vec2 position, float radius, float rotate = 0.0f, glm::vec4 color = glm::vec4(1.0f));
private:
    // number of segments
    size_t numSegments;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData(float percentageOfCircle);
    // only edge
    bool onlyEdge;
};
