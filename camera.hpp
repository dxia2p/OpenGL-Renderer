#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position;
    // Yaw, pitch and fov are in degrees
    float yaw, pitch, sensitivity, fov, nearPlane = 0.2, farPlane = 100.0;
    const float MAX_PITCH = 89.0f, MIN_PITCH = -89.0f;

    Camera(glm::vec3 pos, float aspectRatio) : position(pos), aspectRatio(aspectRatio) {}

    glm::mat4 getLookatMat() const;
    /*!
    * @brief Returns a projection matrix to be passed into a vertex shader
    */
    glm::mat4 getProjectionMat() const;

    /*!
    * @brief Sets the orientation of the camera based on mouse movement
    *
    * @param dx Change in mouse's x coordinate
    * @param dy Change in mouse's y coordinate
    */
    void processMouse(float dx, float dy);

    glm::vec3 getUp() { return up; }
    glm::vec3 getRight() { return right; }
    glm::vec3 getFront() { return front; }
private:
    glm::vec3 up, right, front;
    float aspectRatio;
};
