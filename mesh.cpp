#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>

#include "mesh.hpp"


Mesh::Mesh(glm::vec3 position, std::shared_ptr<Material> material, const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) : position(position), material(material), indexCount(indices.size()) {
    // Create vertex array and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Send vertex data to vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    // Assume the shader takes in vertices, indices, then UVs
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Send index data to element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}


glm::mat4 Mesh::getModelMatrix() {
    glm::mat4 result = glm::mat4(1.0);
    result = glm::scale(result, scale);
    result = glm::mat4_cast(rotation) * result;
    result = glm::translate(result, position);
    return result;
}
