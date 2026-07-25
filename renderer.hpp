#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "skybox.hpp"


class Renderer {
public:
    static constexpr unsigned int MAX_LIGHT_COUNT = 16;
    static constexpr unsigned int MATRICES_UBO_BINDING_POINT = 0;
    static constexpr unsigned int LIGHTS_UBO_BINDING_POINT = 1;

    Renderer();
    void draw(std::vector<Mesh> &meshes, std::vector<Light *> &lights);

    void setCamera(Camera *camera) { this->camera = camera; }

    void setSkybox(Skybox *skybox) { this->skybox = skybox; }
private:
    unsigned int matricesUBO, lightsUBO;
    Camera *camera = nullptr;
    Skybox *skybox = nullptr;
};

#endif
