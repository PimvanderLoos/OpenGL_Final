#version 330 core

// Define constants
#define M_PI 3.141593

uniform mat4 pmvTransform;
uniform uint textured;
uniform uint debug;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform int underWater;

uniform sampler2D map_Kd;
uniform sampler2D map_Ks;
uniform sampler2D map_Bump;

uniform float ns;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 ke;
uniform float ne;
uniform float d;
uniform uint illumModel; // Not used yet, but part of MTL spec.
uniform vec3 cameraPosition;

in vec3 N;
in vec3 vertCoordinates;
in vec2 textureCoordinates;
in vec3 position;

out vec4 fColor;

const uint textured_Kd   = 1u;
const uint textured_Ks   = 2u;
const uint textured_Bump = 4u;

void main()
{
    // Normal / bump mapping
    vec3 normal;
    if (0u != (textured & textured_Bump))
        normal = normalize(2.0 * texture(map_Bump, textureCoordinates).rgb - 1.0);
    else
        normal = normalize(N);

    vec3 pos  = vec3(pmvTransform * vec4(vertCoordinates, 1.0));  // Hit point on object
    vec3 camP = vec3(-pmvTransform * vec4(vertCoordinates, 1.0)); // Camera position
    vec3 s = normalize(lightPos - pos);
    vec3 v = normalize(-pos.xyz);
    float sdn = dot(s, normal);

    vec3 lightDir = normalize(lightPos - vertCoordinates);

    vec3 iA = lightCol * ka;
    vec3 iD = lightCol * kd * max(sdn, 0.0);

    vec3 spec;
    if (0u != (textured & textured_Ks))
        spec = vec3(texture(map_Ks, textureCoordinates));
    else
        spec = ks;

    vec3 iS;
    if (max(dot(lightDir, normal), 0.0) > 0.0f)
    {
        vec3 reflectDir = reflect(lightDir, normal);
        vec3 viewDir = normalize(-position);
        iS = clamp(lightCol * spec * pow(max(dot(reflectDir, viewDir), 0.0), ns), 0.0, 1.0);
    }
    else // Light source on the wrong side.
        iS = vec3(0,0,0);

    if (0u != (textured & textured_Kd))
        fColor = texture(map_Kd, textureCoordinates) * vec4(iA + iD, 1.0) + vec4(iS, 1.0);
    else
        fColor = vec4(0.45, 0.5, 0.6, 1.0);

    if (debug == 1u)
        fColor = vec4(0.5, 0.0, 0.5, 1.0);
}