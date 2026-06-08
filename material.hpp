#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <glm/glm.hpp>
#include "shader.hpp"
#include <glad/glad.h>
#include <iostream>

constexpr int TEXTURE_TYPES_SIZE = 2;
enum class TextureType {
    Diffuse = 0,
    Specular,
    None
};

struct Texture {
    unsigned int id;
    TextureType textureType;
    std::string path;
};

bool operator==(Texture &t1, Texture &t2);

const std::string SHADER_DIFFUSE_MAP_NAMING_CONVENTION = "material.diffuseTexture";
const std::string SHADER_SPECULAR_MAP_NAMING_CONVENTION = "material.specularTexture";
const GLenum DIFFUSE_TEXTURE_UNIT = GL_TEXTURE0;
const GLenum SPECULAR_TEXTURE_UNIT = GL_TEXTURE1;

class Material {
public:
    Material() {}
    Material(float shininess, struct Texture diffuseMap, struct Texture specularMap, Shader *shader, glm::vec3 color = glm::vec3(1));
    float shininess;
    Shader *shader;
    glm::vec3 color;

    unsigned int getDiffuseTextureID() { 
        if (diffuseMap.textureType == TextureType::None) std::cerr << "Attempt to fetch ID of diffuse texture with type 'None'" << std::endl;
        return diffuseMap.id; 
    }
    unsigned int getSpecularTextureID() { 
        if (diffuseMap.textureType == TextureType::None) std::cerr << "Attempt to fetch ID of specular texture with type 'None'" << std::endl;
        return specularMap.id; 
    }
    void setDiffuseTexture(struct Texture t) { diffuseMap = t; } 
    void setSpecularTexture(struct Texture t) { specularMap = t; }

    bool operator==(Material &m);

private:
    struct Texture diffuseMap;
    struct Texture specularMap;
};



#endif
