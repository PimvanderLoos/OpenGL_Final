#version 330 core

in vec3 N;

out vec4 fColor;

void main()
{
    fColor = vec4(N,1.0);
}
