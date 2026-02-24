#include <cstdio>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "camera.hpp"
#include "shader.hpp"
#include "model.hpp"

constexpr float SCR_WIDTH = 800;
constexpr float SCR_HEIGHT = 600;

float deltaTime = 0;
float prevFrameTime = 0;

bool holdingRightClick = false;

float cameraMoveSpeed = 4.0f;
Camera camera = Camera(glm::vec3(0.0, 0.0, 5.0), SCR_WIDTH/SCR_HEIGHT);

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
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

    glm::vec2 moveDir = glm::vec2(0);
    if (glfwGetKey(window, GLFW_KEY_W)) {
        moveDir.y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        moveDir.y -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        moveDir.x -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        moveDir.x += 1;
    }

    if (moveDir.x != 0 || moveDir.y != 0)
        moveDir = glm::normalize(moveDir);
    moveDir *= cameraMoveSpeed;
    camera.position += deltaTime * moveDir.y * camera.getFront();
    camera.position += deltaTime * moveDir.x * camera.getRight();
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    static double lastX, lastY;
    if (holdingRightClick)
        camera.processMouse(xpos - lastX, ypos - lastY);
    lastX = xpos;
    lastY = ypos;
}

int main() {
    // -------------------------------------------- GLFW --------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // -------------------------------------------- ImGui --------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // -------------------------------------------- Shaders and Models --------------------------------------------
    std::string shaderBasePath = std::string(ASSETS_DIR) + "shaders/"; 
    Shader lightSourceShader = Shader(shaderBasePath + "vertexShader.vert", shaderBasePath + "lightSource.frag");

    Shader litShader = Shader(shaderBasePath + "vertexShader.vert", shaderBasePath + "lit.frag");

    std::string modelBasePath = std::string(ASSETS_DIR) + "models/";
    Model model = Model(modelBasePath + "Torus.obj");

    glEnable(GL_DEPTH_TEST);
    while(!glfwWindowShouldClose(window)) {
        deltaTime = glfwGetTime() - prevFrameTime;
        prevFrameTime = glfwGetTime();
        
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw stuff

        glm::mat4 view = camera.getLookatMat();
        glm::mat4 projection = camera.getProjectionMat();
        model.draw(litShader, view, projection);

        // ...
        ImGui::Begin("My name is window, ImGUI window", NULL, ImGuiWindowFlags_NoSavedSettings);
        ImGui::Text("Hello there adventurer");
        ImGui::Text("Camera position: (%f, %f, %f)", camera.position.x, camera.position.y, camera.position.z);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
