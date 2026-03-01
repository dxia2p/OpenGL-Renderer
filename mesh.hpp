#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>
#include "material.hpp"


std::string getShaderTextureUniformName(TextureTypes type, int index);


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};

#endif
