#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform vec2 redChannelRange;
uniform vec2 greenChannelRange;
uniform vec2 blueChannelRange;
uniform vec2 alphaChannelRange;

void main()
{             
    vec4 texColor = texture(image, TexCoords);
    if(texColor.r < redChannelRange.x || texColor.r > redChannelRange.y ||
	texColor.g < greenChannelRange.x || texColor.g > greenChannelRange.y || 
	texColor.b < blueChannelRange.x || texColor.b > blueChannelRange.y ||
	texColor.a < alphaChannelRange.x || texColor.a > alphaChannelRange.y)
        discard;
    FragColor = texColor;
}