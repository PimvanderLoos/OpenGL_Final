#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 uv_in;

// Specify the Uniforms of the vertex shader
uniform mat4  pmvTransform;
uniform mat3  normalTransform;
//uniform float time;

// Specify the output of the vertex stage
out vec2  uv;

void main()
{
    vec3 currentPosition = vertCoordinates_in;

    gl_Position = pmvTransform * vec4(currentPosition, 1.0);
    uv = uv_in;
}
