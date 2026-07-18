#include "renderer.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer() {
    // Generate matricesUBO
    glGenBuffers(1, &matricesUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, MATRICES_UBO_BINDING_POINT, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
    // Generate lightsUBO
    glGenBuffers(1, &lightsUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_UBO_BINDING_POINT, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData) * MAX_LIGHT_COUNT, NULL, GL_STREAM_DRAW);

    glEnable(GL_DEPTH_TEST);
}


void Renderer::draw(std::vector<Mesh> &meshes, std::vector<Light*> &lights) {
    // Check if camera is null
    if (camera == nullptr) std::cerr << "Camera is null in renderer!" << std::endl;

    // Set UBO for view and projection matrices
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->getProjectionMat()));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->getLookatMat()));

    // Set light UBO
    if (lights.size() > MAX_LIGHT_COUNT) std::cerr << "Number of lights cannot exceed " << MAX_LIGHT_COUNT << std::endl;
    struct LightData lightDataArr[MAX_LIGHT_COUNT] = {};
    for(int i = 0; i < MAX_LIGHT_COUNT; i++) {  // Call generateLightData on each light and store them in lightDataArr
        if (i < lights.size()) {
            lightDataArr[i] = lights[i]->generateLightData();
        } else {
            lightDataArr[i] = getNullLight();
        }
    }
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData) * MAX_LIGHT_COUNT, lightDataArr);

    // Loop through the given meshes and draw them
    for(int i = 0; i < meshes.size(); i++) {
        glBindVertexArray(meshes[i].getVAO());
        Shader *shader = meshes[i].material->shader;
        shader->use();

        // Set model matrix
        shader->setMat4("model", meshes[i].getModelMatrix());
        
        // Set camera position
        shader->setVec3("cameraPos", camera->position);

        // Set material uniform
        shader->setVec3("material.color", meshes[i].material->color);
        shader->setInt("material.diffuseTexture", DIFFUSE_TEXTURE_UNIT - GL_TEXTURE0);
        shader->setInt("material.specularTexture", SPECULAR_TEXTURE_UNIT - GL_TEXTURE0);
        shader->setFloat("material.shininess", meshes[i].material->shininess);

        // Bind textures
        glActiveTexture(DIFFUSE_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, meshes[i].material->getDiffuseTextureID());
        glActiveTexture(SPECULAR_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, meshes[i].material->getSpecularTextureID());

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, meshes[i].getIndexCount(), GL_UNSIGNED_INT, 0);
    }
}
