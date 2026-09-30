#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"


enum class LightTypes {
    DIRECTIONAL = 0,
    POINT = 1,
    SPOT = 2,
};

// This mimics the LightData struct inside fragment shaders
// It will be used to pass data to the fragment shader via a uniform buffer object
struct LightData {  // Needs to be aligned to 16 byte boundaries because we use std140 layout in the shader
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec4 ambientDiffuseSpecularLightType;  // Multipliers for ambient, diffuse and specular, w = light type (follows enum class LightTypes)
    alignas(16) glm::vec4 cutoffsAndAttenuation;  // x is inner cutoff (radians), y is outer cutoff (radians), z is linear term for attenuation equation, w is quadratic term
};

// Returns a lightData object that represents a light that doesn't exist
// Need this because setting all fields in LightData to zero results in "direction" being zero, leading to undefined behaviour when the shader attempts to normalize direction
struct LightData getNullLight();

// Base class for lights
class Light {
public:
    virtual ~Light() = default;

    virtual void setAmbient(float val) {
        ambient = val;
    };
    virtual void setDiffuse(float val) {
        diffuse = val;
    }
    virtual void setSpecular(float val) {
        specular = val;
    }

    virtual LightData generateLightData() const = 0;

    virtual std::vector<glm::mat4> getViewAndProjectionMatrices(Camera &camera) const = 0;

    LightTypes lightType;

    // TODO: Make these vary based on camera properties
    float nearPlane = 1.0f, farPlane = 50.0f;  

protected:
    Light(glm::vec3 color, float ambient, float diffuse, float specular, LightTypes lightType) : color(color), ambient(ambient), diffuse(diffuse), specular(specular), lightType(lightType) {}
    glm::vec3 color;
    float ambient, diffuse, specular;
private:
};


class DirectionalLight : public Light {
public:
    DirectionalLight(glm::vec3 color, float ambient, float diffuse, float specular, glm::vec3 direction) : Light(color, ambient, diffuse, specular, LightTypes::DIRECTIONAL), direction(direction) {}

    LightData generateLightData() const override {
        struct LightData result;
        result.position = glm::vec3(0);
        result.direction = direction;
        result.color = color;
        result.ambientDiffuseSpecularLightType = glm::vec4(ambient, diffuse, specular, lightType);
        result.cutoffsAndAttenuation = glm::vec4(0);
        return result;
    }

    std::vector<glm::mat4> getViewAndProjectionMatrices(Camera &camera) const override {
        std::vector<glm::mat4> result;
        glm::mat4 projection = glm::ortho(-frustumWidth, frustumWidth, -frustumHeight, frustumHeight, nearPlane, farPlane);
        glm::vec3 frustumPos = -glm::normalize(direction) * 20.0f;  // TODO: Fix this magic number
        glm::mat4 view = glm::lookAt(frustumPos, frustumPos + direction, glm::vec3(0.0f, 1.0f, 0.0f));
        result.push_back(projection * view);
        return result;
    }

    float frustumWidth = 20.0f, frustumHeight = 20.0f;

    glm::vec3 direction;

private:
};

class PointLight : public Light {
public:
    PointLight(glm::vec3 color, float ambient, float diffuse, float specular, glm::vec3 position, float linear, float quadratic) : Light(color, ambient, diffuse, specular, LightTypes::POINT), position(position), linear(linear), quadratic(quadratic) {}

    LightData generateLightData() const override {
        struct LightData result;
        result.position = position;
        result.direction = glm::vec3(0);
        result.color = color;
        result.ambientDiffuseSpecularLightType = glm::vec4(ambient, diffuse, specular, lightType);
        result.cutoffsAndAttenuation = glm::vec4(0, 0, linear, quadratic);
        return result;
    }

    std::vector<glm::mat4> getViewAndProjectionMatrices(Camera &camera) const override {
        std::vector<glm::mat4> result;
        return result;
    }

    glm::vec3 position;
    float linear, quadratic;

private:
};

class SpotLight : public Light {
public:
    SpotLight(glm::vec3 color, float ambient, float diffuse, float specular, glm::vec3 position, glm::vec3 direction, float linear, float quadratic, float innerCutoff, float outerCutoff) : Light(color, ambient, diffuse, specular, LightTypes::SPOT), position(position), direction(direction), linear(linear), quadratic(quadratic), innerCutoff(innerCutoff), outerCutoff(outerCutoff) {}

    LightData generateLightData() const override {
        struct LightData result;
        result.position = position;
        result.direction = direction;
        result.color = color;
        result.ambientDiffuseSpecularLightType = glm::vec4(ambient, diffuse, specular, lightType);
        result.cutoffsAndAttenuation = glm::vec4(innerCutoff, outerCutoff, linear, quadratic);
        return result;
    }

    std::vector<glm::mat4> getViewAndProjectionMatrices(Camera &camera) const override {
        std::vector<glm::mat4> result;
        return result;
    }

    glm::vec3 position;
    glm::vec3 direction;
    float linear, quadratic, innerCutoff, outerCutoff;

private:
    };

#endif
