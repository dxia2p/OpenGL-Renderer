#version 460 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    /*
    // Calculate our own depth values for the shadowmap (linear depth values)
    float lightDistance = length(FragPos.xyz - lightPos);

    lightDistance = lightDistance / farPlane;

    gl_FragDepth = lightDistance;
    */
}