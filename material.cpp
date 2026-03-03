#include "material.hpp"

bool operator==(Texture &t1, Texture &t2) {
    return (t1.id == t2.id) && (t1.textureType == t2.textureType) && (t1.path == t2.path);
}

bool Material::operator==(Material &m) {
    return (this->shininess == m.shininess) && (this->color == m.color) && (this->shader == m.shader) && (this->diffuseMap == m.diffuseMap) && (this->specularMap == m.specularMap);
}
