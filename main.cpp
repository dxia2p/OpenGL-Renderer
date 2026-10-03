#include <algorithm>
#include <cctype>
#include <filesystem>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "camera.hpp"
#include "light.hpp"
#include "modelLoader.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "renderer.hpp"
#include "skybox.hpp"
#include "window.hpp"

float deltaTime = 0;
float prevTime = 0;
bool holdingRightClick = false;

int windowWidth = 1280, windowHeight = 720;

Camera cam(glm::vec3(0, 0, 30), ((float)windowWidth)/windowHeight);

std::string glmVec3ToString(glm::vec3 v) {
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    windowWidth = width, windowHeight = height;
    cam.aspectRatio = (float)windowWidth / windowHeight;
}

void processInput(GLFWwindow *window) {

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    int rmbState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (rmbState == GLFW_PRESS) {
        if (!holdingRightClick) {
            ImGuiIO &io = ImGui::GetIO();
            if (!io.WantCaptureMouse) {
                holdingRightClick = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }
    } else if (rmbState == GLFW_RELEASE) {
        if (holdingRightClick) {
            holdingRightClick = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard) {
        // Camera keyboard movement
        float cameraSpeed = 10.0f;
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

}
void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    static double lastX, lastY;
    if (holdingRightClick)
        cam.processMouse(xpos - lastX, ypos - lastY);
    lastX = xpos;
    lastY = ypos;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

struct ModelEntry {
    std::string displayName;
    std::string fullPath;
};

std::vector<ModelEntry> scanModelFiles(const std::string &baseDir) {
    std::vector<ModelEntry> results;
    std::filesystem::path modelsDir = std::filesystem::path(baseDir) / "models";
    if (!std::filesystem::exists(modelsDir)) {
        return results;
    }

    const std::vector<std::string> validExtensions = {
        ".obj", ".gltf", ".glb", ".fbx", ".dae", ".blend", ".3ds", ".stl", ".ply"
    };

    try {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(modelsDir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });

                bool matches = false;
                for (const auto &validExt : validExtensions) {
                    if (ext == validExt) {
                        matches = true;
                        break;
                    }
                }

                if (matches) {
                    std::error_code ec;
                    std::string relPath = std::filesystem::relative(entry.path(), baseDir, ec).string();
                    if (ec || relPath.empty()) {
                        relPath = entry.path().filename().string();
                    }
                    results.push_back({relPath, entry.path().string()});
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error scanning models folder: " << e.what() << std::endl;
    }

    std::sort(results.begin(), results.end(), [](const ModelEntry &a, const ModelEntry &b) {
        return a.displayName < b.displayName;
    });

    return results;
}

int main() {
    #ifdef DEBUG_MODE
    bool debug = true;
    #else
    bool debug = false;
    #endif

    GLFWwindow *window = createGLFWWindow(windowWidth, windowHeight, "Hello", debug);

    #ifdef DEBUG_MODE
    int flags; 
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    #endif

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    Shader directionalShadowShader(std::string(ASSETS_DIR) + "shaders/shadowmap.vert", std::string(ASSETS_DIR) + "shaders/directionalShadowmap.frag", std::string(ASSETS_DIR) + "shaders/directionalShadowmap.geom");
    Shader pointShadowShader(std::string(ASSETS_DIR) + "shaders/shadowmap.vert", std::string(ASSETS_DIR) + "shaders/pointShadowmap.frag", std::string(ASSETS_DIR) + "shaders/pointShadowmap.geom");
    Renderer renderer(directionalShadowShader, pointShadowShader, windowWidth, windowHeight);
    // ------------------------------------------------------------ Set up skybox ------------------------------------------------------------
    std::vector<std::string> daySkyboxFaces = {
        std::string(ASSETS_DIR) + "skyboxes/SkyboxDay/right.jpg",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxDay/left.jpg",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxDay/top.jpg",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxDay/bottom.jpg",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxDay/front.jpg",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxDay/back.jpg"
    };
    std::vector<std::string> nightSkyboxFaces = {
        std::string(ASSETS_DIR) + "skyboxes/SkyboxNight/right.png",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxNight/left.png",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxNight/top.png",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxNight/bottom.png",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxNight/front.png",
        std::string(ASSETS_DIR) + "skyboxes/SkyboxNight/back.png"
    };
    Shader skyboxShader(std::string(ASSETS_DIR) + "shaders/skybox.vert", std::string(ASSETS_DIR) + "shaders/skybox.frag");
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    Skybox daySkybox(daySkyboxFaces, &skyboxShader, false);
    Skybox nightSkybox(nightSkyboxFaces, &skyboxShader, false);
    int selectedSkyboxIndex = 0; // 0: SkyboxDay, 1: SkyboxNight
    renderer.setSkybox(&daySkybox);

    // ------------------------------------------------------------ Set up mesh, camera and shaders ------------------------------------------------------------
    Shader shader(std::string(ASSETS_DIR) + "shaders/vertexShader.vert", std::string(ASSETS_DIR) + "shaders/fragmentShader.frag");
    ModelLoader loader;

    std::unique_ptr<DirectionalLight> dirLight = std::make_unique<DirectionalLight>(glm::vec3(1, 1, 1), 0.1f, 0.7f, 0.3f, glm::vec3(0, -0.4, 1));
    glm::vec3 sunDir = glm::normalize(dirLight->direction);
    bool sunLightEnabled = true;
    // 3 Point Lights: Red, Green, Blue positioned > 40 units away from origin
    // Distance from origin: ~43.8 units for Red & Green, ~42.8 units for Blue
    std::unique_ptr<PointLight> redLight = std::make_unique<PointLight>(
        glm::vec3(1.0f, 0.05f, 0.05f), 0.15f, 1.8f, 0.8f, glm::vec3(-35.0f, 8.0f, -25.0f), 0.003f, 0.00005f);
    redLight->farPlane = 150.0f;

    std::unique_ptr<PointLight> greenLight = std::make_unique<PointLight>(
        glm::vec3(0.05f, 1.0f, 0.05f), 0.15f, 1.8f, 0.8f, glm::vec3(35.0f, 8.0f, -25.0f), 0.003f, 0.00005f);
    greenLight->farPlane = 150.0f;

    std::unique_ptr<PointLight> blueLight = std::make_unique<PointLight>(
        glm::vec3(0.05f, 0.2f, 1.0f), 0.15f, 1.8f, 0.8f, glm::vec3(0.0f, 8.0f, 42.0f), 0.003f, 0.00005f);
    blueLight->farPlane = 150.0f;

    bool redLightEnabled = false;
    bool greenLightEnabled = false;
    bool blueLightEnabled = false;

    // Bright overhead spot light shining down on the scene
    std::unique_ptr<SpotLight> spotLight = std::make_unique<SpotLight>(
        glm::vec3(1.0f, 1.0f, 1.0f), 0.05f, 2.5f, 1.5f,
        glm::vec3(0.0f, 30.0f, -5.0f), glm::normalize(glm::vec3(0.0f, -1.0f, 0.0001f)),
        0.005f, 0.0002f, glm::radians(25.0f), glm::radians(35.0f));
    spotLight->nearPlane = 1.0f;
    spotLight->farPlane = 80.0f;

    bool spotLightEnabled = false;

    std::vector<Mesh> spotCube = loader.load(std::string(ASSETS_DIR) + "models/Cube.obj", &shader, false);
    if (!spotCube.empty()) {
        spotCube[0].position = spotLight->position;
        spotCube[0].scale = glm::vec3(0.8f);
        auto mat = std::make_shared<Material>(*spotCube[0].material);
        mat->color = glm::vec3(0.2f, 0.2f, 0.15f);
        mat->shininess = 16.0f;
        spotCube[0].material = mat;
    }

    // Small indicator cubes at the position of each light
    std::vector<Mesh> redCube = loader.load(std::string(ASSETS_DIR) + "models/Cube.obj", &shader, false);
    if (!redCube.empty()) {
        redCube[0].position = redLight->position;
        redCube[0].scale = glm::vec3(1.0f);
        auto mat = std::make_shared<Material>(*redCube[0].material);
        mat->color = glm::vec3(0.15f, 0.02f, 0.02f);
        mat->shininess = 16.0f;
        redCube[0].material = mat;
    }

    std::vector<Mesh> greenCube = loader.load(std::string(ASSETS_DIR) + "models/Cube.obj", &shader, false);
    if (!greenCube.empty()) {
        greenCube[0].position = greenLight->position;
        greenCube[0].scale = glm::vec3(1.0f);
        auto mat = std::make_shared<Material>(*greenCube[0].material);
        mat->color = glm::vec3(0.02f, 0.15f, 0.02f);
        mat->shininess = 16.0f;
        greenCube[0].material = mat;
    }

    std::vector<Mesh> blueCube = loader.load(std::string(ASSETS_DIR) + "models/Cube.obj", &shader, false);
    if (!blueCube.empty()) {
        blueCube[0].position = blueLight->position;
        blueCube[0].scale = glm::vec3(1.0f);
        auto mat = std::make_shared<Material>(*blueCube[0].material);
        mat->color = glm::vec3(0.02f, 0.05f, 0.15f);
        mat->shininess = 16.0f;
        blueCube[0].material = mat;
    }

    // Default cube (floor)
    std::vector<Mesh> floorCube = loader.load(std::string(ASSETS_DIR) + "models/Cube.obj", &shader, false);
    if (!floorCube.empty()) {
        floorCube[0].scale = glm::vec3(50.0f, 1.0, 50.0f);
        floorCube[0].position = glm::vec3(0.0f, -11.0f, 0.0f);
        floorCube[0].material->shininess = 64.0f;
    }

    std::vector<ModelEntry> scannedModels = scanModelFiles(ASSETS_DIR);
    int selectedModelIndex = -1;
    for (int i = 0; i < (int)scannedModels.size(); ++i) {
        if (scannedModels[i].displayName.find("Tokyo") != std::string::npos) {
            selectedModelIndex = i;
            break;
        }
    }
    if (selectedModelIndex == -1 && !scannedModels.empty()) {
        selectedModelIndex = 0;
    }

    bool flipUVs = false;
    glm::vec3 modelPos(0.0f);
    float modelScale = 1.0f;
    glm::vec3 modelRot(0.0f);

    std::vector<Mesh> currentModelMeshes;
    std::vector<Mesh> meshes;

    auto updateSceneMeshes = [&]() {
        meshes.clear();
        meshes.insert(meshes.end(), currentModelMeshes.begin(), currentModelMeshes.end());
        meshes.insert(meshes.end(), floorCube.begin(), floorCube.end());
        if (!spotCube.empty()) meshes.insert(meshes.end(), spotCube.begin(), spotCube.end());
        if (!redCube.empty()) meshes.insert(meshes.end(), redCube.begin(), redCube.end());
        if (!greenCube.empty()) meshes.insert(meshes.end(), greenCube.begin(), greenCube.end());
        if (!blueCube.empty()) meshes.insert(meshes.end(), blueCube.begin(), blueCube.end());
    };

    auto applyTransform = [&]() {
        glm::quat rot = glm::quat(glm::radians(modelRot));
        for (size_t i = 0; i < currentModelMeshes.size(); ++i) {
            currentModelMeshes[i].position = modelPos;
            currentModelMeshes[i].scale = glm::vec3(modelScale);
            currentModelMeshes[i].rotation = rot;
        }
        for (size_t i = 0; i < currentModelMeshes.size() && i < meshes.size(); ++i) {
            meshes[i].position = modelPos;
            meshes[i].scale = glm::vec3(modelScale);
            meshes[i].rotation = rot;
        }
    };

    auto loadSelectedModel = [&](int index) {
        if (index >= 0 && index < (int)scannedModels.size()) {
            std::cout << "Loading model: " << scannedModels[index].fullPath << std::endl;
            ModelLoader modelLoader;
            currentModelMeshes = modelLoader.load(scannedModels[index].fullPath, &shader, flipUVs);
            updateSceneMeshes();
            applyTransform();
        }
    };

    if (selectedModelIndex >= 0) {
        loadSelectedModel(selectedModelIndex);
    } else {
        updateSceneMeshes();
    }

    renderer.setCamera(&cam);
    
    while (!glfwWindowShouldClose(window)) {
        deltaTime = glfwGetTime() - prevTime;
        prevTime = glfwGetTime();

        processInput(window);

        // Start Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ---------------- ImGui Interface ----------------
        ImGui::Begin("3D Renderer Controls");

        if (ImGui::CollapsingHeader("Model Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char *currentPreview = (selectedModelIndex >= 0 && selectedModelIndex < (int)scannedModels.size())
                ? scannedModels[selectedModelIndex].displayName.c_str()
                : "None";

            if (ImGui::BeginCombo("Model", currentPreview)) {
                for (int i = 0; i < (int)scannedModels.size(); i++) {
                    const bool isSelected = (selectedModelIndex == i);
                    if (ImGui::Selectable(scannedModels[i].displayName.c_str(), isSelected)) {
                        if (selectedModelIndex != i) {
                            selectedModelIndex = i;
                            loadSelectedModel(selectedModelIndex);
                        }
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Rescan Models Folder")) {
                std::string currentSelectedPath = (selectedModelIndex >= 0 && selectedModelIndex < (int)scannedModels.size())
                    ? scannedModels[selectedModelIndex].fullPath : "";
                scannedModels = scanModelFiles(ASSETS_DIR);
                selectedModelIndex = -1;
                for (int i = 0; i < (int)scannedModels.size(); ++i) {
                    if (scannedModels[i].fullPath == currentSelectedPath) {
                        selectedModelIndex = i;
                        break;
                    }
                }
            }

            if (ImGui::Checkbox("Flip UVs on Load", &flipUVs)) {
                if (selectedModelIndex >= 0) {
                    loadSelectedModel(selectedModelIndex);
                }
            }

            if (selectedModelIndex >= 0 && selectedModelIndex < (int)scannedModels.size()) {
                ImGui::Text("Current: %s", scannedModels[selectedModelIndex].displayName.c_str());
                ImGui::Text("Meshes loaded: %zu", currentModelMeshes.size());
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "No model loaded.");
            }
        }

        if (ImGui::CollapsingHeader("Model Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            bool transformChanged = false;
            if (ImGui::DragFloat3("Position", &modelPos.x, 0.05f)) transformChanged = true;
            if (ImGui::DragFloat("Scale", &modelScale, 0.01f, 0.001f, 100.0f, "%.3f")) transformChanged = true;
            if (ImGui::DragFloat3("Rotation", &modelRot.x, 1.0f, -180.0f, 180.0f, "%.1f deg")) transformChanged = true;

            if (transformChanged) {
                applyTransform();
            }

            if (ImGui::Button("Reset Transform")) {
                modelPos = glm::vec3(0.0f);
                modelScale = 1.0f;
                modelRot = glm::vec3(0.0f);
                applyTransform();
            }
        }

        if (ImGui::CollapsingHeader("Sun Light (Directional)", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Enable Sun Light", &sunLightEnabled);
            ImGui::SameLine();
            ImGui::TextColored(sunLightEnabled ? ImVec4(1.0f, 1.0f, 0.6f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                               sunLightEnabled ? "[Active]" : "[Inactive]");

            ImGui::Spacing();
            static bool useAngles = false;
            static float azimuth = 180.0f;
            static float elevation = 21.8f;

            bool sunChanged = false;
            ImGui::RadioButton("Vector Controls", (int*)&useAngles, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Angle Controls", (int*)&useAngles, 1);

            if (!useAngles) {
                if (ImGui::SliderFloat("Sun Dir X", &sunDir.x, -1.0f, 1.0f)) sunChanged = true;
                if (ImGui::SliderFloat("Sun Dir Y (Downward)", &sunDir.y, -1.0f, -0.01f)) sunChanged = true;
                if (ImGui::SliderFloat("Sun Dir Z", &sunDir.z, -1.0f, 1.0f)) sunChanged = true;
                if (sunDir.y > -0.01f) {
                    sunDir.y = -0.01f;
                    sunChanged = true;
                }
            } else {
                if (ImGui::SliderFloat("Azimuth", &azimuth, 0.0f, 360.0f, "%.1f deg")) sunChanged = true;
                if (ImGui::SliderFloat("Downward Angle", &elevation, 1.0f, 90.0f, "%.1f deg")) sunChanged = true;
                if (sunChanged) {
                    float radAz = glm::radians(azimuth);
                    float radEl = glm::radians(elevation);
                    sunDir.x = std::sin(radAz) * std::cos(radEl);
                    sunDir.y = -std::sin(radEl); // strictly negative / downward
                    sunDir.z = std::cos(radAz) * std::cos(radEl);
                }
            }

            if (sunChanged) {
                if (sunDir.y > -0.01f) sunDir.y = -0.01f; // strict enforcement: always downwards
                if (glm::length(sunDir) > 0.0001f) {
                    dirLight->direction = glm::normalize(sunDir);
                }
            }

            ImGui::Text("Current Direction: (%.2f, %.2f, %.2f)", dirLight->direction.x, dirLight->direction.y, dirLight->direction.z);

            if (ImGui::Button("Reset Sun Direction")) {
                sunDir = glm::normalize(glm::vec3(0.0f, -0.4f, 1.0f));
                dirLight->direction = sunDir;
                azimuth = 180.0f;
                elevation = 21.8f;
            }
        }

        if (ImGui::CollapsingHeader("Point Lights (RGB)", ImGuiTreeNodeFlags_DefaultOpen)) {
            static float rgbIntensity = 1.8f;
            bool lightsToggled = false;

            if (ImGui::Checkbox("Red Light", &redLightEnabled)) lightsToggled = true;
            ImGui::SameLine();
            ImGui::ColorButton("RedColor", ImVec4(1.0f, 0.05f, 0.05f, 1.0f), ImGuiColorEditFlags_NoTooltip, ImVec2(16, 16));

            if (ImGui::Checkbox("Green Light", &greenLightEnabled)) lightsToggled = true;
            ImGui::SameLine();
            ImGui::ColorButton("GreenColor", ImVec4(0.05f, 1.0f, 0.05f, 1.0f), ImGuiColorEditFlags_NoTooltip, ImVec2(16, 16));

            if (ImGui::Checkbox("Blue Light", &blueLightEnabled)) lightsToggled = true;
            ImGui::SameLine();
            ImGui::ColorButton("BlueColor", ImVec4(0.05f, 0.2f, 1.0f, 1.0f), ImGuiColorEditFlags_NoTooltip, ImVec2(16, 16));

            if (ImGui::SliderFloat("Light Intensity", &rgbIntensity, 0.1f, 5.0f, "%.2f")) {
                redLight->setDiffuse(rgbIntensity);
                greenLight->setDiffuse(rgbIntensity);
                blueLight->setDiffuse(rgbIntensity);
            }

            ImGui::Spacing();
            if (ImGui::Button("All On")) {
                redLightEnabled = greenLightEnabled = blueLightEnabled = true;
                lightsToggled = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("All Off")) {
                redLightEnabled = greenLightEnabled = blueLightEnabled = false;
                lightsToggled = true;
            }

            if (lightsToggled) {
                if (!redCube.empty()) redCube[0].material->color = redLightEnabled ? glm::vec3(1.0f, 0.05f, 0.05f) : glm::vec3(0.15f, 0.02f, 0.02f);
                if (!greenCube.empty()) greenCube[0].material->color = greenLightEnabled ? glm::vec3(0.05f, 1.0f, 0.05f) : glm::vec3(0.02f, 0.15f, 0.02f);
                if (!blueCube.empty()) blueCube[0].material->color = blueLightEnabled ? glm::vec3(0.05f, 0.2f, 1.0f) : glm::vec3(0.02f, 0.05f, 0.15f);
                updateSceneMeshes();
                applyTransform();
            }
        }

        if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Enable Overhead Spot Light", &spotLightEnabled)) {
                if (!spotCube.empty()) spotCube[0].material->color = spotLightEnabled ? glm::vec3(1.0f, 1.0f, 0.8f) : glm::vec3(0.2f, 0.2f, 0.15f);
                updateSceneMeshes();
                applyTransform();
            }
            ImGui::SameLine();
            ImGui::TextColored(spotLightEnabled ? ImVec4(1.0f, 1.0f, 0.6f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                               spotLightEnabled ? "[Active]" : "[Inactive]");
        }

        if (ImGui::CollapsingHeader("Skybox Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char *skyboxOptions[] = { "Day (SkyboxDay)", "Night (SkyboxNight)" };
            if (ImGui::Combo("Skybox", &selectedSkyboxIndex, skyboxOptions, IM_ARRAYSIZE(skyboxOptions))) {
                if (selectedSkyboxIndex == 0) {
                    renderer.setSkybox(&daySkybox);
                } else {
                    renderer.setSkybox(&nightSkybox);
                }
            }
        }

        if (ImGui::CollapsingHeader("Scene & Performance")) {
            ImGui::Text("FPS: %.1f (%.2f ms/frame)", io.Framerate, 1000.0f / io.Framerate);
            ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", cam.position.x, cam.position.y, cam.position.z);
            ImGui::Text("Window Size: %d x %d", windowWidth, windowHeight);
        }

        if (ImGui::CollapsingHeader("Controls Help")) {
            ImGui::BulletText("Hold Right Mouse Button: Look around");
            ImGui::BulletText("W / A / S / D: Move camera");
            ImGui::BulletText("ESC: Close application");
        }

        ImGui::End();

        /* LOGIC */
        //spotLight->direction = cam.getFront();
        //spotLight->position = cam.position;

        /* END OF LOGIC */

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.windowHeight = windowHeight;
        renderer.windowWidth = windowWidth;

        std::vector<Light *> activeLights;
        if (sunLightEnabled) activeLights.push_back(dirLight.get());
        if (spotLightEnabled) activeLights.push_back(spotLight.get());
        if (redLightEnabled) activeLights.push_back(redLight.get());
        if (greenLightEnabled) activeLights.push_back(greenLight.get());
        if (blueLightEnabled) activeLights.push_back(blueLight.get());

        renderer.draw(meshes, activeLights);

        // Render Dear ImGui
        ImGui::Render();
        GLboolean srgbEnabled = glIsEnabled(GL_FRAMEBUFFER_SRGB);
        if (srgbEnabled) {
            glDisable(GL_FRAMEBUFFER_SRGB);
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (srgbEnabled) {
            glEnable(GL_FRAMEBUFFER_SRGB);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}
