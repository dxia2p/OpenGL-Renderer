#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include "model.hpp"
#include "light.hpp"

class Renderer {
public:
    static constexpr int MAX_LIGHT_COUNT = 32;

    Renderer();

    void render(const std::vector<ModelInstance> &modelInstances, const std::vector<LightData> &lightDataList);

    void setCamera(Camera *camera) { this->camera = camera; }

private:
    unsigned int uboMatrices, uboLights;
    Camera *camera;
};

#endif
