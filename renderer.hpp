#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "skybox.hpp"

enum class ObjectShaderUniformLocation : GLint {
    ModelMatrix = 0,
    NormalMatrix = 1,
    DirectionalShadowMaps = 2,
    CameraPos = 3,
    MaterialColor = 4,
    MaterialDiffuseSampler = 5,
    MaterialSpecularSampler = 6,
    MaterialShininess = 7,
};

enum class ShadowShaderUniformLocation : GLint {
    ModelMatrix = 0,
};

enum class TextureUnits : GLenum {
    Diffuse = GL_TEXTURE0,
    Specular = GL_TEXTURE1,
    DirectionalShadowmaps = GL_TEXTURE2,
};

class Renderer {
public:
    static constexpr unsigned int MAX_LIGHT_COUNT = 16;
    static constexpr unsigned int MATRICES_UBO_BINDING_POINT = 0;
    static constexpr unsigned int LIGHTS_UBO_BINDING_POINT = 1;
    static constexpr unsigned int LIGHTS_PROJ_VIEW_MAT_UBO_BINDING_POINT = 2;
    static constexpr unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

    Renderer(Shader shadowShader);
    void draw(std::vector<Mesh> &meshes, std::vector<Light *> &lights);

    void setCamera(Camera *camera) { this->camera = camera; }

    void setSkybox(Skybox *skybox) { this->skybox = skybox; }
private:
    unsigned int fallbackDiffuseTex, fallbackSpecularTex;
    unsigned int matricesUBO, lightsUBO, lightsProjViewMatsUBO;
    Camera *camera = nullptr;
    Skybox *skybox = nullptr;
    unsigned int shadowMapsFBO;
    unsigned int shadowMaps;
    Shader shadowShader;
};

#endif
