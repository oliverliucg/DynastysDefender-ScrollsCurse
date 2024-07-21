#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform bool  chaos;
uniform bool  confuse;
uniform bool  shake;
uniform float time;
uniform float shakingStrength;
uniform float timeMultiplierForX;
uniform float timeMultiplierForY;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0f, 1.0f); 
    vec2 texture = vertex.zw;
    if (chaos)
    {
        //float strength = 0.3;
        //vec2 pos = vec2(texture.x + sin(time) * strength, texture.y + cos(time) * strength);        
        //TexCoords = pos;
	TexCoords = texture;
    }
    else if (confuse)
    {
        TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);
    }
    else
    {
        TexCoords = texture;
    }
    if (shake)
    {
        gl_Position.x += cos(time * timeMultiplierForX) * shakingStrength;        
        gl_Position.y += cos(time * timeMultiplierForY) * shakingStrength;        
    }
} 