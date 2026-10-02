#version 460 core

#define MAX_LIGHT_COUNT 16

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(std140, binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
};
layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat3 normalMatrix;

layout(std140, binding = 2) uniform directionalShadowMatrices {
    mat4 lightSpaceMatrices[MAX_LIGHT_COUNT];  // These matrices must correspond to the lights given in the lights arrays in the fragment shader
};

out vec3 FragPos; // Position of the fragment in world coordinates
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace[MAX_LIGHT_COUNT];

void main() {
    gl_Position = projection * view * modelMatrix * vec4(aPos, 1.0);
    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    TexCoord = aTexCoords;

    // Transform fragments into the view space of each light
    for(int i = 0; i < MAX_LIGHT_COUNT; i++) {
        FragPosLightSpace[i] = lightSpaceMatrices[i] * vec4(FragPos, 1.0);
    }
}
