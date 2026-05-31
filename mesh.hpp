#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>

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
    Mesh(glm::vec3 position, std::shared_ptr<Material> material, const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

    glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), scale = glm::vec3(1.0, 1.0, 1.0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    //std::vector<Vertex> vertices;
    //std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;

    unsigned int getVAO() { return VAO; }
    unsigned int getIndexCount() { return indexCount; }
    glm::mat4 getModelMatrix();
private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};


#endif
