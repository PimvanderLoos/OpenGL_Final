#version 330 core

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;

// Specify the Uniforms of the vertex shader
uniform mat4 pmvTransform;
uniform mat3 normalTransform;

out vec3 N;

void main()
{
    gl_Position = pmvTransform * vec4(vertCoordinates_in, 1.0);
    N           = normalize(normalTransform * vertNormal_in);
}
