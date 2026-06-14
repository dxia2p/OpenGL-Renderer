#include "light.hpp"
struct LightData getNullLight() {
    struct LightData result;
    result.position = glm::vec3(0);
    result.direction = glm::vec3(123, 456, 789);
    result.color = glm::vec3(0);
    result.ambientDiffuseSpecularLightType = glm::vec4(0);
    result.cutoffsAndAttenuation = glm::vec4(0);
    return result;
}
