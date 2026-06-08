#include "renderer.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include <iostream>
#include <algorithm>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer() {
    // Generate matricesUBO
    glGenBuffers(1, &matricesUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, MATRICES_UBO_BINDING_POINT, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 0, 0, GL_STATIC_DRAW); // TODO: FIx this
    // Generate lightsUBO
    glGenBuffers(1, &lightsUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_UBO_BINDING_POINT, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData), NULL, GL_STATIC_DRAW);

    glEnable(GL_DEPTH_TEST);
}


void Renderer::draw(std::vector<Mesh> &meshes, std::vector<Light> &lights) {
    // Set matrices UBO

    // Set light UBO
    if (lights.size() > MAX_LIGHT_COUNT) std::cerr << "Number of lights cannot exceed " << MAX_LIGHT_COUNT << std::endl;
    struct LightData lightDataArr[MAX_LIGHT_COUNT];
    for(int i = 0; i < std::min((unsigned int)lights.size(), MAX_LIGHT_COUNT); i++) {  // Call generateLightData on each light and store them in lightDataArr
        lightDataArr[i] = lights[i].generateLightData();
    }
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData) * MAX_LIGHT_COUNT, lightDataArr);

    // Loop through the given meshes and set shader uniforms before drawing the mesh
    for(int i = 0; i < meshes.size(); i++) {
        glBindVertexArray(meshes[i].getVAO());
        Shader *shader = meshes[i].material->shader;
        shader->use();
        // Matrices
        shader->setMat4("model", meshes[i].getModelMatrix());
        shader->setMat4("view", camera->getLookatMat());  // TEMPORARY
        shader->setMat4("projection", camera->getProjectionMat());  // TEMPORARY
        // Textures
        glActiveTexture(DIFFUSE_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, meshes[i].material->getDiffuseTextureID());
        glActiveTexture(SPECULAR_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, meshes[i].material->getSpecularTextureID());
        glDrawElements(GL_TRIANGLES, meshes[i].getIndexCount(), GL_UNSIGNED_INT, 0);
    }
}
