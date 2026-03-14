#ifndef MESH_H
#define MESH_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "material.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh(glm::vec3 position, Material *material, const std::vector<Vertex> &vertices, const std::vector<Vertex> &indices);

    glm::vec3 position, scale;
    glm::quat rotation;
    //std::vector<Vertex> vertices;
    //std::vector<unsigned int> indices;
    Material *material;
private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};


#endif
