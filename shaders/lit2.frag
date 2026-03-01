#version 460 core

#define MAX_LIGHT_COUNT 32

struct LightData {
    vec3 position;
    vec3 direction;
    vec4 colorAndIntensity;  // w is intensity
    vec3 ambientDiffuseSpecular;  // Multipliers for ambient, diffuse and specular
    vec4 cutoffsAndAttenuation;  // x is inner cutoff, y is outer cutoff, z is linear term for attenuation equation, w is quadratic term
    int lightType;
};

layout (std140, binding = 1) uniform LightBlock {
    LightData lights[MAX_LIGHT_COUNT];
    int activeLightCount;
}

void main() {
    
}
