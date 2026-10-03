#version 460 core

#define MAX_LIGHT_COUNT 16

in vec3 FragPos; // Position of the fragment in world coordinates
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace[MAX_LIGHT_COUNT];  // The position of the current fragment in the view (?) space of each light

layout(location = 2) uniform sampler2DArray directionalShadowMaps;

layout(location = 8) uniform samplerCubeArray pointShadowMaps;

layout(location = 3) uniform vec3 cameraPos;

struct Material {
    vec3 color;
    sampler2D diffuseTexture;
    sampler2D specularTexture;
    float shininess;
};
layout(location = 4) uniform Material material;

// Lights

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


// Functions
/* A modified version of the step() function that returns 0.0 if x <= edge, and 1.0 if x > edge */
float modifiedStep(float edge, float x) {
    return 1.0 - step(x, edge);
}

// Returns 1.0 if fragment is in shadow, 0.0 otherwise
float shadowCalculationDirectional(sampler2DArray shadowMaps, int shadowMapIndex, vec4 fragPosLightSpace, vec3 lightDir) {
    vec3 ndc = fragPosLightSpace.xyz / fragPosLightSpace.w;
    vec3 shadowmapCoords = ndc * 0.5 + 0.5;
    float closestDepth = texture(shadowMaps, vec3(shadowmapCoords.xy, shadowMapIndex)).r;
    float currentDepth = shadowmapCoords.z;
    
    float bias = max(0.05 * (1.0 - dot(normalize(Normal), normalize(-lightDir))), 0.005);

    float shadow = closestDepth < (currentDepth - bias) ? 1.0 : 0.0;
    shadow *= 1.0 - step(1.0, shadowmapCoords.z);
    return shadow;
}

// 1.0 if fragment in shadow, 0.0 otherwise
float shadowCalculationPoint(samplerCubeArray shadowMaps, int shadowMapIndex, LightData light, vec3 fragPos) {
    vec3 fragToLight = fragPos - light.position;  // Can this ever be zero?
    float closestDepth = texture(shadowMaps, vec4(fragToLight, shadowMapIndex)).r;

    closestDepth *= light.nearFarPlane.y;
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

/* Function that returns the color of a fragment after a directional light shines on it */
vec3 calcDirLight(LightData light, vec3 viewDir, int lightIndex) {
    float diffuseMult = max(dot(normalize(Normal), normalize(-light.direction)), 0);
    vec3 diffuse = diffuseMult * vec3(light.ambientDiffuseSpecularLightType.y) * texture(material.diffuseTexture, TexCoord).rgb;

    /* Blinn phong */
    vec3 halfwayDir = normalize(-normalize(light.direction) - normalize(viewDir));
    float specularMult = pow(max(dot(halfwayDir, normalize(Normal)), 0), material.shininess);
    specularMult *= modifiedStep(0.0, diffuseMult);

    vec3 specular = specularMult * vec3(light.ambientDiffuseSpecularLightType.z) * texture(material.specularTexture, TexCoord).rgb;

    vec3 ambient = vec3(light.ambientDiffuseSpecularLightType.x) * vec3(texture(material.diffuseTexture, TexCoord));

    float shadow = shadowCalculationDirectional(directionalShadowMaps, lightIndex, FragPosLightSpace[lightIndex], light.direction);
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * material.color * light.color;
}

/* Function that returns the color of a fragment after a point light shines on it */
vec3 calcPointLight(LightData light, vec3 viewDir, int lightIndex) {
    vec3 lightDir = normalize(FragPos - light.position);
    float diffuseMult = max(dot(normalize(Normal), -lightDir), 0.0);

    /* Blinn phong */
    vec3 halfwayDir = normalize(normalize(-lightDir) - normalize(viewDir));
    float specularMult = pow(max(dot(halfwayDir, normalize(Normal)), 0.0), material.shininess);
    specularMult *= modifiedStep(0.0, diffuseMult);

    vec3 ambient = light.ambientDiffuseSpecularLightType.x * vec3(texture(material.diffuseTexture, TexCoord));
    vec3 diffuse = diffuseMult * light.ambientDiffuseSpecularLightType.y * vec3(texture(material.diffuseTexture, TexCoord));
    vec3 specular = specularMult * light.ambientDiffuseSpecularLightType.z * vec3(texture(material.specularTexture, TexCoord));

    float dist = distance(light.position, FragPos);
    float attenuation = 1.0 / (1 + light.cutoffsAndAttenuation.z * dist + light.cutoffsAndAttenuation.w * dist * dist);

    float shadow = shadowCalculationPoint(pointShadowMaps, lightIndex, light, FragPos);
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * material.color * light.color * attenuation;
}

/* Function that returns the color of a fragment after a spot light shines on it */
vec3 calcSpotLight(LightData light, vec3 viewDir, int lightIndex) {
    vec3 lightDir = normalize(FragPos - light.position);  // Direction from light to fragment
    float diffuseMult = max(dot(normalize(Normal), -lightDir), 0.0);

    vec3 halfwayDir = normalize(normalize(-lightDir) - normalize(viewDir));
    float specularMult = pow(max(dot(halfwayDir, normalize(Normal)), 0.0), material.shininess);
    specularMult *= modifiedStep(0.0, diffuseMult);

    // We will assume that cutoff values in LightData are in radians
    float fragAngle = dot(lightDir, normalize(light.direction));   // Angle between lightDir (direction from light to fragment) and the direction of the spot light
    // light.cutoffsAndAttenuation.x is inner cutoff and y is outer cutoff
    float intensity = clamp((fragAngle - cos(light.cutoffsAndAttenuation.y)) / (cos(light.cutoffsAndAttenuation.x) - cos(light.cutoffsAndAttenuation.y)), 0.0, 1.0);  // Intensity is used to create soft edges

    diffuseMult *= intensity;
    specularMult *= intensity;
    
    vec3 ambient = light.ambientDiffuseSpecularLightType.x * vec3(texture(material.diffuseTexture, TexCoord));
    vec3 diffuse = diffuseMult * light.ambientDiffuseSpecularLightType.y * vec3(texture(material.diffuseTexture, TexCoord));
    vec3 specular = specularMult * light.ambientDiffuseSpecularLightType.z * vec3(texture(material.specularTexture, TexCoord));

    float dist = distance(light.position, FragPos);
    float attenuation = 1.0 / (1 + light.cutoffsAndAttenuation.z * dist + light.cutoffsAndAttenuation.w * dist * dist);

    float shadow = shadowCalculationDirectional(directionalShadowMaps, lightIndex, FragPosLightSpace[lightIndex], light.direction);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * material.color * light.color * attenuation;
}

out vec4 FragColor;

void main() {
    FragColor = vec4(0, 0, 0, 1);

    for(int i = 0; i < MAX_LIGHT_COUNT; i++) {
        if (lights[i].ambientDiffuseSpecularLightType.w == 0) {  // Directional light
            FragColor += vec4(calcDirLight(lights[i], FragPos - cameraPos, i), 0);
        } else if (lights[i].ambientDiffuseSpecularLightType.w == 1) {  // Point light
            FragColor += vec4(calcPointLight(lights[i], FragPos - cameraPos, i), 0);
        } else {  // Spot light
            FragColor += vec4(calcSpotLight(lights[i], FragPos - cameraPos, i), 0);
        }
    }

    // FragColor = vec4(Normal, 1.0);
}
