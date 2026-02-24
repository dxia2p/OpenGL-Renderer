#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.hpp"

glm::mat4 Camera::getLookatMat() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMat() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::processMouse(float dx, float dy) {
    pitch -= dy * sensitivity;
    yaw += dx * sensitivity;
    
    pitch = glm::clamp(pitch, MIN_PITCH, MAX_PITCH);
    yaw = std::fmod(yaw, 360);

    front = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    front = glm::normalize(front);
    
    right = glm::normalize(glm::cross(front, up));
}

