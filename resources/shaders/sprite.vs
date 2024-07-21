#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 tPos;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform int textureMode; // 0: normal, 1: flip horizontally, etc.

void main()
{
    switch (textureMode) {
        case 1: // Flip horizontally
            TexCoords = vec2(1.0 - tPos.x, tPos.y);
            break;
        default: // No flip
            TexCoords = tPos.xy;
            break;
    }
    gl_Position = projection * model * vec4(aPos, 1.0);
}