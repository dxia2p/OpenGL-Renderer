#include "renderer.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer(Shader directionalShadowShader, Shader pointShadowShader, int windowWidth, int windowHeight) : directionalShadowShader(directionalShadowShader), 
        pointShadowShader(pointShadowShader), windowWidth(windowWidth), windowHeight(windowHeight) {
    
    // Generate matricesUBO
    glGenBuffers(1, &matricesUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(UBOBindingPoints::CamMatrices), matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);

    // Generate lightsUBO
    glGenBuffers(1, &lightsUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(UBOBindingPoints::Lights), lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData) * MAX_LIGHT_COUNT, NULL, GL_STREAM_DRAW);

    // Generate directionalShadowMatsUBO
    glGenBuffers(1, &directionalShadowMatsUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(UBOBindingPoints::DirectionalShadows), directionalShadowMatsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * MAX_LIGHT_COUNT, NULL, GL_STREAM_DRAW);

    // Generate pointShadowMatsUBO
    glGenBuffers(1, &pointShadowMatsUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(UBOBindingPoints::PointShadows), pointShadowMatsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * MAX_LIGHT_COUNT * 6, NULL, GL_STREAM_DRAW);

    // Create fallback textures
    uint8_t rgba[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &fallbackDiffuseTex);
    glGenTextures(1, &fallbackSpecularTex);

    glBindTexture(GL_TEXTURE_2D, fallbackDiffuseTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

    glBindTexture(GL_TEXTURE_2D, fallbackSpecularTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);  // Gamma correction
    
    // Initialize directional shadow maps
    glGenTextures(1, &directionalShadowMapArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, directionalShadowMapArray);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32, DIRECTIONAL_SHADOW_WIDTH, DIRECTIONAL_SHADOW_HEIGHT, MAX_LIGHT_COUNT);  // Immutable storage
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glGenFramebuffers(1, &directionalShadowMapsFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, directionalShadowMapsFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, directionalShadowMapArray, 0);
    glDrawBuffer(GL_NONE);  // Do not render to color buffer
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Initialize point shadow maps
    glGenTextures(1, &pointShadowCubemapArray);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, pointShadowCubemapArray);
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32, POINT_SHADOW_WIDTH, POINT_SHADOW_HEIGHT, MAX_LIGHT_COUNT * 6);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER); 
    glTexParameterfv(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    glGenFramebuffers(1, &pointShadowMapsFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapsFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointShadowCubemapArray, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::draw(std::vector<Mesh> &meshes, std::vector<Light*> &lights) {
    // Check if camera is null
    if (camera == nullptr) std::cerr << "Camera is nullptr in renderer!" << std::endl;
    if (skybox == nullptr) std::cerr << "Skybox is nullptr in renderer!" << std::endl;

    // --------------------------------- Set Lights UBO (Point shadow maps need this as well as the normal fragment shader, so I will put it here) -------------------------------

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

    // ----------------------------------------------------------------------- Render shadow maps ---------------------------------------------------------------------

    // Render shadowmaps for directional shadows
    glViewport(0, 0, DIRECTIONAL_SHADOW_WIDTH, DIRECTIONAL_SHADOW_HEIGHT);

    glm::mat4 directionalShadowMatrices[MAX_LIGHT_COUNT] = {};
    for(int i = 0; i < std::min((size_t)MAX_LIGHT_COUNT, lights.size()); i++) {
        if (lights[i]->lightType == LightTypes::DIRECTIONAL || lights[i]->lightType == LightTypes::SPOT) {
            directionalShadowMatrices[i] = lights[i]->getViewAndProjectionMatrices(*camera)[0];
        }
    }
    glBindBuffer(GL_UNIFORM_BUFFER, directionalShadowMatsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * MAX_LIGHT_COUNT, directionalShadowMatrices);

    directionalShadowShader.use();

    // Render all directional light shadow maps in one pass with a geometry shader
    glBindFramebuffer(GL_FRAMEBUFFER, directionalShadowMapsFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    for(int i = 0; i < meshes.size(); i++) {
        directionalShadowShader.setMat4(static_cast<GLint>(ShadowShaderUniformLocation::ModelMatrix), meshes[i].getModelMatrix());
        glBindVertexArray(meshes[i].getVAO());
        glDrawElements(GL_TRIANGLES, meshes[i].getIndexCount(), GL_UNSIGNED_INT, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Render shadowmaps for point shadows
    glViewport(0, 0, POINT_SHADOW_WIDTH, POINT_SHADOW_HEIGHT);

    glm::mat4 pointShadowMatrices[MAX_LIGHT_COUNT * 6] = {};
    for(int i = 0; i < std::min((size_t)MAX_LIGHT_COUNT, lights.size()); i++) {
        if (lights[i]->lightType == LightTypes::POINT) {
            std::vector<glm::mat4> mats = lights[i]->getViewAndProjectionMatrices(*camera);
            for(int matIdx = 0; matIdx < 6; matIdx++) {
                pointShadowMatrices[i * 6 + matIdx] = mats[matIdx];
            }
        }
    }
    /*
    glBindBuffer(GL_UNIFORM_BUFFER, static_cast<GLuint>(UBOBindingPoints::PointShadows));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * MAX_LIGHT_COUNT * 6, pointShadowMatrices);
    */

    pointShadowShader.use();

    // Render point shadow maps one by one
    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapsFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    for(int i = 0; i < std::min((size_t)MAX_LIGHT_COUNT, lights.size()); i++) {
        pointShadowShader.setInt(static_cast<GLint>(ShadowShaderUniformLocation::PointShadowLightIndex), i);
        for(int j = 0; j < 6; j++) {
            pointShadowShader.setMat4(static_cast<GLint>(ShadowShaderUniformLocation::PointShadowMatrices) + j, pointShadowMatrices[i * 6 + j]);
        }

        for(int i = 0; i < meshes.size(); i++) {
            pointShadowShader.setMat4(static_cast<GLint>(ShadowShaderUniformLocation::ModelMatrix), meshes[i].getModelMatrix());
            glBindVertexArray(meshes[i].getVAO());
            glDrawElements(GL_TRIANGLES, meshes[i].getIndexCount(), GL_UNSIGNED_INT, 0);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ------------------------------------------------------------------------ Render meshes -----------------------------------------------------------------------------------
   glViewport(0, 0, windowWidth, windowHeight);  // TODO: CHANGE THIS

    // Set UBO for view and projection matrices
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->getProjectionMat()));  // First matrix in UBO is projection
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->getLookatMat()));  // second matrix is view

    // Loop through the given meshes and draw them
    for(int i = 0; i < meshes.size(); i++) {
        glBindVertexArray(meshes[i].getVAO());
        Shader *shader = meshes[i].material->shader;
        shader->use();

        // Set model matrix
        glm::mat4 modelMatrix = meshes[i].getModelMatrix();
        shader->setMat4(static_cast<GLint>(ObjectShaderUniformLocation::ModelMatrix), modelMatrix);

        // Set normal matrix
        glm::mat4 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
        shader->setMat3(static_cast<GLint>(ObjectShaderUniformLocation::NormalMatrix), normalMatrix);
        
        // Set camera position
        shader->setVec3(static_cast<GLint>(ObjectShaderUniformLocation::CameraPos), camera->position);

        // Set material uniform
        shader->setVec3(static_cast<GLint>(ObjectShaderUniformLocation::MaterialColor), meshes[i].material->color);
        shader->setInt(static_cast<GLint>(ObjectShaderUniformLocation::MaterialDiffuseSampler), static_cast<GLenum>(TextureUnits::Diffuse) - GL_TEXTURE0);
        shader->setInt(static_cast<GLint>(ObjectShaderUniformLocation::MaterialSpecularSampler), static_cast<GLenum>(TextureUnits::Specular) - GL_TEXTURE0);
        shader->setFloat(static_cast<GLint>(ObjectShaderUniformLocation::MaterialShininess), meshes[i].material->shininess);

        // Bind textures
        glActiveTexture(static_cast<GLenum>(TextureUnits::Diffuse));
        glBindTexture(GL_TEXTURE_2D, meshes[i].material->hasDiffuseTexture() ? meshes[i].material->getDiffuseTextureID() : fallbackDiffuseTex);
        glActiveTexture(static_cast<GLenum>(TextureUnits::Specular));
        glBindTexture(GL_TEXTURE_2D, meshes[i].material->hasSpecularTexture() ? meshes[i].material->getSpecularTextureID() : fallbackSpecularTex);

        /*
        // Set uniforms for calculating shadows
        for(int i = 0; i < lights.size(); i++) {
            if (lights[i]->lightType == LightTypes::DIRECTIONAL) {
                shader->setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lights[i]->getViewAndProjectionMatrices(*camera)[0]);
            }
        }
            */
        shader->setInt(static_cast<GLint>(ObjectShaderUniformLocation::DirectionalShadowMaps), static_cast<GLenum>(TextureUnits::DirectionalShadowmaps) - GL_TEXTURE0);
        glActiveTexture(static_cast<GLenum>(TextureUnits::DirectionalShadowmaps));
        glBindTexture(GL_TEXTURE_2D_ARRAY, directionalShadowMapArray);
        shader->setInt(static_cast<GLint>(ObjectShaderUniformLocation::PointShadowCubemaps), static_cast<GLenum>(TextureUnits::PointShadowCubemaps) - GL_TEXTURE0);
        glActiveTexture(static_cast<GLenum>(TextureUnits::PointShadowCubemaps));
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, pointShadowCubemapArray);

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, meshes[i].getIndexCount(), GL_UNSIGNED_INT, 0);
    }

    // ------------------------------------------------------------------------- Render skybox --------------------------------------------------------------------------------------------------

    // Render the skybox last
    glDepthMask(GL_FALSE);  // Disable depth writing to ensure skybox is always drawn behind other objects
    skybox->shader->use();
    skybox->shader->setMat4("projection", camera->getProjectionMat());
    skybox->shader->setMat4("view", glm::mat4(glm::mat3(camera->getLookatMat())));  // Remove the translation section of the view matrix for skyboxes
    glBindVertexArray(skybox->getVAO());
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getCubemap());
    glDrawArrays(GL_TRIANGLES, 0, SKYBOX_VERT_COUNT);
    glDepthMask(GL_TRUE);
}
