#version 460 core

layout(triangles) in;
layout (triangle_strip, max_vertices = 18) out;

layout(location = 2) uniform mat4 shadowMatrices[6];

layout(location = 1) uniform int lightIndex;

out vec4 FragPos;  // Use this to calculate our own linear depth values

void main() {
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face + lightIndex * 6;

        for (int i = 0; i < 3; ++i) {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}