#version 330 core

// Define constants
#define M_PI 3.141593

in vec2 textureCoordinates;
in vec3 iA;
in vec3 iD;
in vec3 iS;

uniform sampler2D map_Kd;
uniform uint textured;
const uint textured_Kd = 1u;

out vec4 fColor;

void main()
{
    if (0u != (textured & textured_Kd))
        fColor = texture(map_Kd, textureCoordinates) * vec4(iA + iD, 1.0) + vec4(iS, 1.0);
    else
        fColor = vec4(0.45, 0.5, 0.6, 1.0);
}
