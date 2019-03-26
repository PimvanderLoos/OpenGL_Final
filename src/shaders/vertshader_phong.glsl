#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoordinates_in;

// Specify the Uniforms of the vertex shader
uniform mat4 pmvTransform;
uniform mat4 modelTransform;
uniform mat3 normalTransform;
uniform vec4 clipPlane;

out vec3 N;
out vec3 vertCoordinates;
out vec2 textureCoordinates;
out vec3 position;

void main()
{
    position           = vec3(modelTransform * vec4(vertCoordinates_in, 1.0));
    gl_Position        = pmvTransform * vec4(vertCoordinates_in, 1.0);
    gl_ClipDistance[0] = dot(modelTransform * vec4(vertCoordinates_in, 1.0), clipPlane);
    N                  = normalize(normalTransform * vertNormal_in);
    vertCoordinates    = vertCoordinates_in;
    textureCoordinates = textureCoordinates_in;
}
