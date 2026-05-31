cmake_minimum_required(VERSION 3.18)

project(GLRenderer)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


if(${TEST})

    set(SOURCES testMain.cpp thirdparty/glad.c shader.cpp mesh.cpp camera.cpp material.cpp modelLoader.cpp)

else()

    SET(SOURCES main.cpp thirdparty/glad.c shader.cpp mesh.cpp camera.cpp material.cpp modelLoader.cpp)

endif()

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")

target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/includes)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm::glm assimp::assimp imgui::imgui)

