#version 330 core
out vec4 FragColor;

struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform DirectionalLight directionalLight;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 Normal);

void main() {
    FragColor = vec4(calculateDirectionalLight(directionalLight, Normal), 1.0);
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 Normal) {
    vec3 result = vec3(0);

    // Ambient
    result += light.ambient;

    
    // Calculate diffuse
    vec3 lightDir = normalize(-light.direction);
    vec3 normalizedNormal = normalize(Normal);
    float cosAngle = dot(lightDir, normalizedNormal);
    result += max(cosAngle, 0.0) * light.diffuse;
    

    // Calculate specular

    return result;
}
