#version 460 core

#define MAX_LIGHT_COUNT 16

in vec4 FragPos;

struct LightData {  // Everything should be set to 0 to represent a light that doesn't exist
    vec3 position;
    vec3 direction;
    vec3 color;
    vec4 ambientDiffuseSpecularLightType;  // xyz = multipliers for ambient, diffuse, specular, w = type of light (see light.hpp)
    vec4 cutoffsAndAttenuation;  // x = inner cutoff, y = outer cutoff, z = linear term for attenuation, w = quadratic term for attenuation
    vec4 nearFarPlane;  // x = near plane, y = far plane, z and w are padding
};
layout(std140, binding = 1) uniform Lights {
    LightData lights[MAX_LIGHT_COUNT];
};

void main() {
    int lightIndex = gl_Layer / 6;

// Calculate our own linear depth values
    float lightDistance = length(FragPos.xyz - lights[lightIndex].position);
    lightDistance = lightDistance / lights[lightIndex].nearFarPlane.y;
    gl_FragDepth = lightDistance;
}