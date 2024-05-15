#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "ResourceManager.h"

class Renderer
{
public:
    // Constructor (inits shaders/shapes)
    Renderer(const Shader& shader);
    // Virtual Destructor
    virtual ~Renderer();

    // Get the expected window size padding.
    static SizePadding GetExpectedWindowSizePadding();
    // Get the actual window size padding.
    static SizePadding GetActualWindowSizePadding(); 
    // Set the expected window size padding.
    static void SetExpectedWindowSizePadding(SizePadding sizePadding);
    // Set the actual window size padding.
    static void SetActualWindowSizePadding(SizePadding sizePadding);

protected:
    // Render state
    Shader       shader;
    unsigned int VAO;
    unsigned int VBO;

    static SizePadding expected_window_size_padding_;
    static SizePadding actual_window_size_padding_;
};