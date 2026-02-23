cmake_minimum_required(VERSION 3.18)

project(GLRenderer)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(SOURCES main.cpp thirdparty/glad.c shader.cpp model.cpp mesh.cpp camera.cpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")

target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/includes)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm::glm assimp::assimp imgui::imgui)
