#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <glm/glm.hpp>
#include "shader.hpp"

constexpr int TEXTURE_TYPES_SIZE = 2;
enum class TextureTypes {
    Diffuse = 0,
    Specular
};

struct Texture {
    unsigned int id;
    TextureTypes textureType;
    std::string path;
};

bool operator==(Texture &t1, Texture &t2);

const std::string SHADER_DIFFUSE_MAP_NAMING_CONVENTION = "textureDiffuse";
const std::string SHADER_SPECULAR_MAP_NAMING_CONVENTION = "textureSpecular";

class Material {
public:
    Material() {}
    Material(float shininess, struct Texture diffuseMap, struct Texture specularMap, Shader *shader, glm::vec3 color = glm::vec3(1)) : shininess(shininess), diffuseMap(diffuseMap), specularMap(specularMap), shader(shader), color(color) {}
    float shininess;
    struct Texture diffuseMap;
    struct Texture specularMap;
    Shader *shader;
    glm::vec3 color;

    bool operator==(Material &m);
};



#endif
