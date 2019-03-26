#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

in vec4 position;
in vec2 textureCoords;
in vec3 viewDir_w;
in vec3 fromLight_w;
in vec4 worldPos;

uniform sampler2D refractionTexture;
uniform sampler2D reflectionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float time;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform mat4 pmvTransform;

const float amplitude = 0.08;
const float ns = 32.0;
const float specular = 0.35;

// TODO: Make these uniform variables. This is dumb.
const float nearPlane = 0.01f;
const float farPlane = 1000.0f;

// Fresnel approximation as taken from:
// https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter19.html
const float fresnelBias = 0.25f;
const float fresnelPow = 12.0f;

float computeFresnel(float NdotL)
{
    return max(fresnelBias + (1.0f - fresnelBias) * pow((1.0f - NdotL), fresnelPow), 0.0f);
}

float linearize(float value)
{
//    return 2.0f * nearPlane * farPlane / (farPlane + nearPlane - (2.0f * value - 1.0f) * (farPlane - nearPlane));
    float linear = nearPlane / (farPlane - value * (farPlane - nearPlane)) * farPlane;
    return (linear * 2.0) - 1.0;
}

void main()
{
    // Get normalized device coordinates from clipspace. i.e. bottom left is [0,0], top right [1,1].
    vec2 ndc = ((position.xy / position.w) + 1.0) / 2.0;

    // Get reflection and refraction coordinates from the ndc coordinates. Reflection has negative y because it's a reflection. (i.e. inverted).
    vec2 reflectionCoords = vec2(ndc.x, -ndc.y);
    vec2 refractionCoords = vec2(ndc.x,  ndc.y);

    // Get the difference in distance between the distance of the camera to the object behind the water (as taken
    // from the depthMap). and the distance of the camera to the given point on the water.
    // You do not get the actual difference in height, but rather the amount of water between the surface and the point
    // on the object behind it.
    float waterDepth = linearize(texture(depthMap, refractionCoords).r) - linearize(gl_FragCoord.z);
    float waterDepthDampener = clamp(waterDepth / 8.0, 0.0, 1.0);

    // The coordinates should get distorted according to the dudv map, so it actually looks like water.
    // Otherwise, it'll just look like a semi-transparent mirror.
    // Also dampen the total distortion based on the water depth.
    vec2 distortedCoords = texture(dudvMap, vec2(textureCoords.x + time, textureCoords.y)).rg * 0.1;
    distortedCoords = textureCoords + vec2(distortedCoords.x, distortedCoords.y + time);
    vec2 totDistortion = (texture(dudvMap, distortedCoords).rg * 2.0 - 1.0) * amplitude * waterDepthDampener;

    // Distort and then clamp the coordinates. If not clamped, the distortion could cause them to go out of range,
    // resulting in the top of the textures being used, which looks glitchy.
    refractionCoords += totDistortion;
    refractionCoords  = clamp(refractionCoords, 0.001, 0.999);

    reflectionCoords  += totDistortion;
    reflectionCoords.x = clamp(reflectionCoords.x,  0.001,  0.999);
    // y-axis is flipped, so clamping should be negative.
    reflectionCoords.y = clamp(reflectionCoords.y, -0.999, -0.001);

    // Get normal at the current point given the normal map and the coordinates distorted before.
    vec4 normalMapColor = texture(normalMap, distortedCoords);
    vec3 normal = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0));
    vec3 iS = clamp(pow(max(dot(reflect(normalize(fromLight_w), normal), viewDir_w), 0.0), ns) * lightCol * specular, 0.0, 1.0);

    // Fresnell effect:
    vec3 viewDir_w = normalize(viewDir_w);
    float NdotL = max(dot(-viewDir_w, vec3(0.0, 1.0, 0.0)), 0);
    float fresnel = computeFresnel(NdotL);

    fColor = mix(texture(refractionTexture, refractionCoords), texture(reflectionTexture, reflectionCoords), fresnel);
    // Mix in a little bit of blue and add the specular highlights.
    fColor = mix(fColor, vec4(0.0, 0.15, 0.26, 1.0), 0.22) + vec4(iS, 0.0);
    // The shallower water gets, the less visible it becomes. Might be nice to couple it with fresnel in the future.
    fColor.a = waterDepthDampener;
}