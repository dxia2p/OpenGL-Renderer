cmake_minimum_required(VERSION 3.18)

project(renderer)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


# if(${TEST})

#     set(SOURCES testMain.cpp thirdparty/glad.c shader.cpp mesh.cpp camera.cpp material.cpp modelLoader.cpp renderer.cpp light.cpp skybox.cpp)

# else()

#     SET(SOURCES main.cpp thirdparty/glad.c shader.cpp mesh.cpp camera.cpp material.cpp modelLoader.cpp renderer.cpp light.cpp skybox.cpp)

# endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(SOURCES main.cpp thirdparty/glad.c shader.cpp mesh.cpp camera.cpp material.cpp modelLoader.cpp renderer.cpp light.cpp skybox.cpp window.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE)
endif()

# target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS_DIR="./")

target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/includes)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm::glm assimp::assimp imgui::imgui)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()

if (MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
endif()

