#version 330 core

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 uv_in;

// Specify the Uniforms of the vertex shader
uniform mat4 pmvTransform;
uniform mat4 modelTransform;
uniform vec3 cameraPosition;
uniform vec3 lightCol;
uniform vec3 lightPos;

out vec2 textureCoords;
out vec3 viewDir_w;
out vec3 fromLight_w;
out vec4 position;
out vec4 worldPos;

const float frequency = 4.0;

void main()
{
    worldPos = modelTransform * vec4(uv_in.x, 0.0, uv_in.y, 1.0);
    position = pmvTransform * vec4(vertCoordinates_in, 1.0);
    gl_Position = position;
    textureCoords = vec2((uv_in.x + 1.0) / 2.0, uv_in.y) * frequency;
    viewDir_w = normalize(cameraPosition - worldPos.xyz);
    fromLight_w = worldPos.xyz - lightPos;
}
