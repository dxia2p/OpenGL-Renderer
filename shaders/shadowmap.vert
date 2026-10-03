// We can use the same vertex shader for directional and point shadows

#version 460 core

layout (location = 0) in vec3 aPos;

layout(location = 0) uniform mat4 model;

void main() {
    gl_Position = model * vec4(aPos, 1.0);
}