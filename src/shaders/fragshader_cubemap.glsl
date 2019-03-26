#version 330 core

uniform samplerCube skybox;

in vec3 TexCoords;

out vec4 fColor;

void main()
{
    fColor = texture(skybox, -TexCoords);
}