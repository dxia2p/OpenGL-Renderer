#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <memory>

#include "camera.hpp"
#include "modelLoader.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "renderer.hpp"

float deltaTime = 0;
float prevTime = 0;
bool holdingRightClick = false;

Camera cam(glm::vec3(0, 0, 5), 800.0/600);

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    int rmbState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (rmbState == GLFW_PRESS) {
        holdingRightClick = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else if (rmbState == GLFW_RELEASE) {
        holdingRightClick = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    // Camera keyboard movement
    float cameraSpeed = 4.0f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam.position += cam.getFront() * deltaTime * cameraSpeed;       
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam.position -= cam.getFront() * deltaTime * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam.position -= cam.getRight() * deltaTime * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam.position += cam.getRight() * deltaTime * cameraSpeed;
    }

}
void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    static double lastX, lastY;
    if (holdingRightClick)
        cam.processMouse(xpos - lastX, ypos - lastY);
    lastX = xpos;
    lastY = ypos;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef DEBUG_MODE
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    #endif

    GLFWwindow *window = glfwCreateWindow(800, 600, "Test", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    glEnable(GL_DEPTH_TEST);

    // ------------------------------------------------------------ Set up mesh, camera and shaders ------------------------------------------------------------
    Shader shader(std::string(ASSETS_DIR) + "shaders/test.vert", std::string(ASSETS_DIR) + "shaders/test.frag");
    ModelLoader loader;
    std::vector<Mesh> meshes = loader.load(std::string(ASSETS_DIR) + "models/backpack/backpack.obj", &shader);
    std::vector<std::unique_ptr<Light>> lights;
    lights.push_back(std::make_unique<DirectionalLight>(glm::vec3(0, 1, 0), 1.0f, 1.0f, 1.0f, glm::vec3(1, -1, 0)));
    Renderer renderer;
    renderer.setCamera(&cam);
    

    while (!glfwWindowShouldClose(window)) {
        deltaTime = glfwGetTime() - prevTime;
        prevTime = glfwGetTime();

        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.draw(meshes, lights);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
