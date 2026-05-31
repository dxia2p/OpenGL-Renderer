#version 330 core

in vec3 FragPos; // Position of the fragment in world coordinates
in vec3 Normal;

out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
