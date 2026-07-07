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


// Functions
vec3 calcDirLight(LightData light, vec3 viewDir) {

    vec3 diffuse = max(dot(normalize(Normal), normalize(-light.direction)), 0) * vec3(light.ambientDiffuseSpecularLightType.y) * texture(material.diffuseTexture, TexCoord).rgb;
    vec3 reflectDir = normalize(reflect(light.direction, Normal));
    vec3 specular = pow(max(dot(reflectDir, -normalize(viewDir)), 0), material.shininess) * vec3(light.ambientDiffuseSpecularLightType.z) * texture(material.specularTexture, TexCoord).rgb;

    vec3 ambient = vec3(light.ambientDiffuseSpecularLightType.x) * vec3(texture(material.diffuseTexture, TexCoord));

    return (ambient + diffuse + specular) * material.color * light.color;
}


/*
vec3 calcDirLight(LightData light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normalize(-viewDir), reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambientDiffuseSpecularLightType.x * vec3(texture(material.diffuseTexture, TexCoord));
    vec3 diffuse  = light.ambientDiffuseSpecularLightType.y * diff * vec3(texture(material.diffuseTexture, TexCoord));
    vec3 specular = light.ambientDiffuseSpecularLightType.z * spec * vec3(texture(material.specularTexture, TexCoord));
    return (ambient + diffuse + specular) * material.color * light.color;
}  
*/

vec3 calcPointLight(LightData light, vec3 viewDir) {
    vec3 lightDir = normalize(FragPos - light.direction);
    float diffuseMult = max(dot(Normal, lightDir), 0.0);

    vec3 reflectDir = normalize(reflect(lightDir, Normal));
    float specularMult = pow(max(dot(reflectDir, -normalize(viewDir)), 0.0), material.shininess);
    
    vec3 ambient = light.ambientDiffuseSpecularLightType.x * vec3(texture(material.diffuseTexture, TexCoord));
    vec3 diffuse = diffuseMult * light.ambientDiffuseSpecularLightType.y * vec3(texture(material.diffuseTexture, TexCoord));
    vec3 specular = specularMult * light.ambientDiffuseSpecularLightType.z * vec3(texture(material.specularTexture, TexCoord));

    float dist = distance(light.position, FragPos);
    float attenuation = 1.0 / (1 + light.cutoffsAndAttenuation.z * dist + light.cutoffsAndAttenuation.w * dist * dist);

    return (ambient + diffuse + specular) * material.color * light.color * attenuation;
}

vec3 calcSpotLight(LightData light, vec3 viewDir) {
    return vec3(0, 0, 0);
}

out vec4 FragColor;

void main() {
    FragColor = vec4(0, 0, 0, 1);

    for(int i = 0; i < MAX_LIGHT_COUNT; i++) {
        if (lights[i].ambientDiffuseSpecularLightType.w == 0) {  // Directional light
            FragColor += vec4(calcDirLight(lights[i], FragPos - cameraPos), 0);
        } else if (lights[i].ambientDiffuseSpecularLightType.w == 1) {  // Point light
            FragColor += vec4(calcPointLight(lights[i], FragPos - cameraPos), 0);
        } else {  // Spot light
            FragColor += vec4(calcSpotLight(lights[i], FragPos - cameraPos), 0);
        }
    }
}
