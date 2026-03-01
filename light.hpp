#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "shader.hpp"

enum class LightTypes {
    DIRECTIONAL = 0,
    POINT = 1,
    SPOT = 2,
};

// This mimics the LightData struct inside fragment shaders
// It will be used to pass data to the fragment shader VIA a uniform buffer object
struct LightData {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec4 ambientDiffuseSpecularLightType;  // Multipliers for ambient, diffuse and specular, w = light type (follows enum class LightTypes)
    alignas(16) glm::vec4 cutoffsAndAttenuation;  // x is inner cutoff, y is outer cutoff, z is linear term for attenuation equation, w is quadratic term
};

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

protected:
    Light(glm::vec3 color, float ambient, float diffuse, float specular) : color(color), ambient(ambient), diffuse(diffuse), specular(specular) {}
    glm::vec3 color;
    float ambient, diffuse, specular;
private:
};


class DirectionalLight : public Light {
public:
    DirectionalLight(glm::vec3 color, float ambient, float diffuse, float specular, glm::vec3 direction) : Light(color, ambient, diffuse, specular), direction(direction) {}

    LightData generateLightData() const override {
        struct LightData result;
        result.position = glm::vec3(0);
        result.direction = direction;
        result.color = color;
        result.ambientDiffuseSpecularLightType = glm::vec4(ambient, diffuse, specular, LightTypes::DIRECTIONAL);
        result.cutoffsAndAttenuation = glm::vec4(0);
        return result;
    }

private:
    glm::vec3 direction;
};

class PointLight : public Light {
public:
    PointLight(glm::vec3 color, float ambient, float diffuse, float specular, glm::vec3 position, float linear, float quadratic) : Light(color, ambient, diffuse, specular), position(position), linear(linear), quadratic(quadratic) {}

    LightData generateLightData() const override {
        struct LightData result;
        result.position = position;
        result.direction = glm::vec3(0);
        result.color = color;
        result.ambientDiffuseSpecularLightType = glm::vec4(ambient, diffuse, specular, LightTypes::POINT);
        result.cutoffsAndAttenuation = glm::vec4(0, 0, linear, quadratic);
        return result;
    }

private:
    glm::vec3 position;
    float linear, quadratic;
};

class SpotLight : public Light {
public:
    SpotLight(glm::vec3 color, float ambient, float diffuse, float specular, glm::vec3 position, glm::vec3 direction, float linear, float quadratic) : Light(color, ambient, diffuse, specular), position(position), direction(direction), linear(linear), quadratic(quadratic) {}

    LightData generateLightData() const override {
        struct LightData result;
        result.position = position;
        result.direction = direction;
        result.color = color;
        result.ambientDiffuseSpecularLightType = glm::vec4(ambient, diffuse, specular, LightTypes::SPOT);
        result.cutoffsAndAttenuation = glm::vec4(innerCutoff, outerCutoff, linear, quadratic);
        return result;
    }

private:
    glm::vec3 position;
    glm::vec3 direction;
    float linear, quadratic, innerCutoff, outerCutoff;
};

#endif
