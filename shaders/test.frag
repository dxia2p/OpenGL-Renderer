#version 460 core

in vec3 FragPos; // Position of the fragment in world coordinates
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 cameraPos;

struct Material {
    vec3 color;
    sampler2D diffuseTexture;
    sampler2D specularTexture;
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
layout(std140, binding = 1) uniform Lights {
    LightData lights[MAX_LIGHT_COUNT];
};

out vec4 FragColor;

// Functions
vec3 calcDirLight(LightData light, vec3 normal, vec3 viewDir) {
    vec3 ambient = vec3(light.ambientDiffuseSpecularLightType.x);

    vec3 diffuse = max(dot(normalize(Normal), normalize(light.direction)), 0) * vec3(light.ambientDiffuseSpecularLightType.y);
    vec3 reflectDir = reflect(light.direction, Normal);
    vec3 specular = max(dot(reflectDir, -viewDir), 0) * vec3(light.ambientDiffuseSpecularLightType.z);

    return (ambient + diffuse + specular) * material.color * texture(material.diffuseTexture, TexCoord).rgb;
}

void main() {
    FragColor = vec4(0, 0, 0, 1);

    for(int i = 0; i < MAX_LIGHT_COUNT; i++) {
        if (lights[i].ambientDiffuseSpecularLightType.w == 0) {  // Directional light
            FragColor += vec4(calcDirLight(lights[i], Normal, FragPos - cameraPos), 0);
        } else if (lights[i].ambientDiffuseSpecularLightType.w == 1) {  // Point light

        } else {  // Spot light

        }
    }
}
