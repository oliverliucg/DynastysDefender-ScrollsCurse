#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 tPos;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = tPos.xy;
    gl_Position = projection * model * vec4(aPos, 1.0);
}