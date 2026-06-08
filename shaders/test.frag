#version 330 core

in vec3 FragPos; // Position of the fragment in world coordinates
in vec3 Normal;
in vec2 TexCoord;

struct Material {
    vec3 color;
    sampler2D specularTexture;
    sampler2D diffuseTexture;
    float shininess;
};
uniform Material material;

// Lights
#define MAX_LIGHT_COUNT 16
struct LightData {  // Everything should be set to 0 to represent a light that doesn't exist
    vec3 position;
    vec3 direction;
    vec3 color;
    vec4 ambientDiffuseSpecularLightType;  // xyz = multipliers for ambient, diffuse, specular, w = type of light (see light.hpp)
    vec4 cutoffsAndAttenuation;  // x = inner cutoff, y = outer cutoff, z = linear term for attenuation, w = quadratic term for attenuation
};
layout (std140, binding = 1) uniform Lights {
    struct LightData lights[MAX_LIGHT_COUNT];
};

out vec4 FragColor;

void main() {
    FragColor = texture(material.diffuseTexture, TexCoord);
}
