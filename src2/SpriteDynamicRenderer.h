#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Renderer.h"

class SpriteDynamicRenderer : public Renderer
{
public:
    // Constructor (inits shaders/shapes)
    SpriteDynamicRenderer(const Shader& shader);
    // Destructor
    ~SpriteDynamicRenderer();
    // Renders a defined quad textured with given sprite
    void DrawSprite(const Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(100.0f, 100.0f), float rotate = 0.0f, glm::vec2 rotationPivot = glm::vec2(0.5f, 0.5f), glm::vec4 color = glm::vec4(1.0f), glm::vec4 texCoords = glm::vec4(0.f, 0.0f, 1.0f, 1.0f));

private:

    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};
