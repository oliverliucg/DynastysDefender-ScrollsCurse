#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D image;
uniform vec4 spriteColor;
uniform float revealFactor; // Controls how much of the texture is revealed

void main() {
    vec2 adjustedTexCoord = vec2(TexCoord.x, mix(0.5, TexCoord.y, revealFactor));
    FragColor = texture(image, adjustedTexCoord);
}
