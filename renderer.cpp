#include "renderer.hpp"
#include "light.hpp"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer() {
    // Handle uniform buffer for view and projection matrices
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);

    // Create uniform buffer for lights
    glGenBuffers(1, &uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, MAX_LIGHT_COUNT * sizeof(LightData), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLights);
}


void Renderer::render(const std::vector<ModelInstance> &modelInstances, const std::vector<LightData> &lightDataList) {
    // Set view and projection matrices
    if (camera == nullptr) {
        std::cerr << "Renderer::render failed because camera is null!" << std::endl;
        return;
    }
    glm::mat4 view = camera->getLookatMat();
    glm::mat4 projection = camera->getProjectionMat();
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    //glBindBuffer(GL_UNIFORM_BUFFER, 0);


    // Send lights to uboLights
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData) * lightDataList.size(), lightDataList.data());
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    // Render model instances

}
