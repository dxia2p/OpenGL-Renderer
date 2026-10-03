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
    PointShadowCubemaps = 8,
};

enum class ShadowShaderUniformLocation : GLint {
    ModelMatrix = 0,
    PointShadowLightIndex = 1,
    PointShadowMatrices = 2,
};

enum class TextureUnits : GLenum {
    Diffuse = GL_TEXTURE0,
    Specular = GL_TEXTURE1,
    DirectionalShadowmaps = GL_TEXTURE2,
    PointShadowCubemaps = GL_TEXTURE3,
};

enum class UBOBindingPoints : GLuint {
    CamMatrices = 0,
    Lights = 1,
    DirectionalShadows = 2,
    PointShadows = 3,
};

class Renderer {
public:
    static constexpr unsigned int MAX_LIGHT_COUNT = 16;
    static constexpr unsigned int DIRECTIONAL_SHADOW_WIDTH = 2048, DIRECTIONAL_SHADOW_HEIGHT = 2048;
    static constexpr unsigned int POINT_SHADOW_WIDTH = 2048, POINT_SHADOW_HEIGHT = 2048;  // The resolution for one face of a point shadow cubemap

    Renderer(Shader directionalShadowShader, Shader pointShadowShader, int windowHeight, int windowWidth);
    void draw(std::vector<Mesh> &meshes, std::vector<Light *> &lights);

    void setCamera(Camera *camera) { this->camera = camera; }

    void setSkybox(Skybox *skybox) { this->skybox = skybox; }
    
    int windowHeight, windowWidth;
private:


    unsigned int fallbackDiffuseTex, fallbackSpecularTex;
    unsigned int matricesUBO, lightsUBO, directionalShadowMatsUBO, pointShadowMatsUBO;
    Camera *camera = nullptr;
    Skybox *skybox = nullptr;
    unsigned int directionalShadowMapsFBO;
    unsigned int directionalShadowMapArray;
    unsigned int pointShadowMapsFBO;
    unsigned int pointShadowCubemapArray;
    Shader directionalShadowShader;
    Shader pointShadowShader;
};

#endif
