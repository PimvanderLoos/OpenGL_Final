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
uniform uint textured;

uniform vec3 lightPos;
uniform vec3 lightCol;

uniform sampler2D map_Ks;
uniform sampler2D map_Bump;

uniform float ns;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 ke;
uniform float ne;
uniform float d;
uniform uint illumModel;
uniform vec3 cameraPosition;

// Specify the output of the vertex stage
out vec3 iA;
out vec3 iD;
out vec3 iS;
out vec2 textureCoordinates;

const uint textured_Ks   = 2u;
const uint textured_Bump = 4u;

void main()
{
    gl_Position = pmvTransform * vec4(vertCoordinates_in, 1.0);
    gl_ClipDistance[0] = dot(modelTransform * vec4(vertCoordinates_in, 1.0), clipPlane);
    vec3 N   = normalize(normalTransform * vertNormal_in);            // Normal
    vec3 pos = vec3(pmvTransform * vec4(vertCoordinates_in, 1.0));    // Hit point on object
    vec3 V   = normalize(lightPos - pos);                             // View vector
    vec3 L   = lightPos - vertCoordinates_in;                         // Light vector

    float diffuseCoefficient = max(0.0, dot(N, V));
    iA = lightCol * ka;
    iD = lightCol * kd * diffuseCoefficient;

    if (max(dot(L, N), 0.0) > 0.0f)
    {
        vec3 spec;
        if (0u != (textured & textured_Ks))
            spec = vec3(texture(map_Ks, textureCoordinates));
        else
            spec = ks;
        vec3 R     = reflect(L, N);
        float dist = length(lightPos - pos);
        float att  = 1.0 / (1.0 + 64 * pow(d, 2));
        iS = clamp(att * lightCol * spec * pow(max(dot(-R, V), 0.0), ns), 0.0, 0.2);
    }
    else // Light source on the wrong side.
        iS = vec3(0,0,0);

    textureCoordinates = textureCoordinates_in;
}
