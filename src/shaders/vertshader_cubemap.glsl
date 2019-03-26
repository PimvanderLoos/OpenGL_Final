#version 330 core
layout (location = 0) in vec3 vertCoordinates_in;

out vec3 TexCoords;

uniform mat4 pmvTransform;

void main()
{
    TexCoords = vertCoordinates_in;
    gl_Position = pmvTransform * vec4(vertCoordinates_in, 1.0);
}