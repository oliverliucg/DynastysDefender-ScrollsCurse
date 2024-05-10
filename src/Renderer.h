#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

class Renderer
{
public:
    // Constructor (inits shaders/shapes)
    Renderer(const Shader& shader);
    // Virtual Destructor
    virtual ~Renderer();

protected:
    // Render state
    Shader       shader;
    unsigned int VAO;
    unsigned int VBO;
};