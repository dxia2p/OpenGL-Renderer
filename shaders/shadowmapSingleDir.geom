#version 460 core

#define MAX_LIGHT_COUNT 16
layout (triangles) in;
layout (triangle_strip, max_vertices = MAX_LIGHT_COUNT * 3) out;  // 3 vertices * number of lights (layers in the shadowmap)

layout(std140, binding = 2) uniform directionalShadowMatrices {
    mat4 LightProjViewMatrices[MAX_LIGHT_COUNT];  // View and projection matrices for each directional light   
};


out vec4 FragPos;  // Don't need this unless we are calculating our own depth values

void main() {
    for(int layer = 0; layer < MAX_LIGHT_COUNT; layer++) {
        gl_Layer = layer;
        for(int i = 0; i < 3; i++) {
            FragPos = gl_in[i].gl_Position;
            gl_Position = LightProjViewMatrices[layer] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}