#version 330 core
in  vec2  TexCoords;
out vec4  color;
  
uniform sampler2D scene;
uniform vec2      offsets[25];
uniform int       edge_kernel[25];
uniform float     intensity;
uniform float     blur_kernel[25];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform bool blur;
uniform bool grayscale;

void main()
{
    color = vec4(0.0f);
    vec3 sample[25];
    // sample from texture offsets if using convolution matrix
    if(chaos || shake || blur)
        for(int i = 0; i < 25; i++)
            sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));

    // process effects
    vec4 edgeColor = vec4(0.0f);
    if (chaos) {           
        for(int i = 0; i < 25; i++)
            edgeColor += vec4(sample[i] * edge_kernel[i], 0.0f);
        edgeColor.a = 1.0f;
        // Interpolate between original color and edgeColor based on edgeIntensity
        color = mix(texture(scene, TexCoords), edgeColor, intensity);
    }
    else if (confuse)
    {
        color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
    }
    else if (blur)
    {
	for(int i = 0; i < 25; i++)
            color += vec4(sample[i] * blur_kernel[i], 0.0f);
        color.a = 1.0f;	
    }
    else
    {
        color =  texture(scene, TexCoords);
    }

    if (grayscale)
    {
        float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        color = vec4(average, average, average, 1.0);
    }
}