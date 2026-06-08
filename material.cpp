#include "material.hpp"


Material::Material(float shininess, struct Texture diffuseMap, struct Texture specularMap, Shader *shader, glm::vec3 color) : shininess(shininess), diffuseMap(diffuseMap), specularMap(specularMap), shader(shader), color(color) {
    shader->use();
    shader->setInt(SHADER_DIFFUSE_MAP_NAMING_CONVENTION, DIFFUSE_TEXTURE_UNIT - GL_TEXTURE0);
    shader->setInt(SHADER_SPECULAR_MAP_NAMING_CONVENTION, SPECULAR_TEXTURE_UNIT - GL_TEXTURE1);

}

bool operator==(Texture &t1, Texture &t2) {
    return (t1.id == t2.id) && (t1.textureType == t2.textureType) && (t1.path == t2.path);
}

bool Material::operator==(Material &m) {
    return (this->shininess == m.shininess) && (this->color == m.color) && (this->shader == m.shader) && (this->diffuseMap == m.diffuseMap) && (this->specularMap == m.specularMap);
}
