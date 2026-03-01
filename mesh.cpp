#include <glad/glad.h>
#include <stb_image.h>
#include "mesh.hpp"

std::string getShaderTextureUniformName(TextureTypes type, int index) {
    std::string result = "texture_";
    switch (type) {
        case TextureTypes::Diffuse:
            result += "diffuse";
            break;
        case TextureTypes::Specular:
            result += "specular";
            break;
    }
    result += std::to_string(index);
    return result;
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) {
    // Generate buffers and VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Send data to buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Enable vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(offsetof(Vertex, texCoords)));

    glBindVertexArray(0);

    
    // Update indexCount
    indexCount = indices.size();
    // Handle textures
}
