#version 330 core

// Define constants
#define M_PI 3.141593

uniform sampler2D sampler;
in vec2 uv;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    fColor = texture(sampler, uv);
}
