#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Renderer.h"

class PartialTextureRenderer : public Renderer
{
public:
    // Constructor (inits shaders/shapes)
    PartialTextureRenderer(const Shader& shader);
    // Destructor
    ~PartialTextureRenderer();
    // Renders a defined quad textured with given sprite
    void DrawPartialTexture(
        const Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(100.0f, 100.0f),  
        glm::vec2 redChannelRange = glm::vec2(0.f, 1.f), glm::vec2 greenChannelRange = glm::vec2(0.f, 1.f), 
        glm::vec2 blueChannelRange = glm::vec2(0.f, 1.f), glm::vec2 alphaChannelRange = glm::vec2(0.f, 1.f), float rotate = 0.0f, glm::vec2 rotationPivot = glm::vec2(0.5f, 0.5f));

private:

    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};
