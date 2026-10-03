#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <exception>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

namespace {
    // Helper function to read data from a shader file
    std::string readShader(const std::filesystem::path &path) {
        std::ifstream file;
        std::string fileContents;

        // Turn on exceptions for .failed() and .bad()
        file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        
        try {
            file.open(path);

            std::stringstream stream;
            stream << file.rdbuf();
            fileContents = stream.str();
        } catch (const std::exception& e) {
            std::cerr << "Shader file could not be read at " << path.string() << ": " << e.what() << std::endl;
        }

        return fileContents;
    }

    unsigned int createShader(std::string shaderSrcCode, GLenum shaderType, const std::filesystem::path &path /* Only used for error reporting */) {
        const char* shaderCStr = shaderSrcCode.c_str();

        int success;
        char log[512];

        unsigned int shader;
        
        shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderCStr, NULL);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, log);
            std::cerr << "Error compiling shader at " << path.string() << ':' << log << std::endl;
        }

        return shader;
    }
} // namespace

Shader::Shader(const std::filesystem::path &vertexPath, const std::filesystem::path &fragmentPath, const std::filesystem::path &geometryPath) {

    bool hasGeometryShader = !geometryPath.empty();

    std::string vShaderContents = readShader(vertexPath);
    std::string fShaderContents = readShader(fragmentPath);
    std::string gShaderContents = hasGeometryShader ? readShader(geometryPath) : "";

    unsigned int vShader = createShader(vShaderContents, GL_VERTEX_SHADER, vertexPath);
    unsigned int fShader = createShader(fShaderContents, GL_FRAGMENT_SHADER, fragmentPath);
    unsigned int gShader = hasGeometryShader ? createShader(gShaderContents, GL_GEOMETRY_SHADER, geometryPath) : 0;

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    if (hasGeometryShader) glAttachShader(shaderProgram, gShader);
    glLinkProgram(shaderProgram);

    int success;
    char log[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, log);
        std::cerr << "Error linking shader program: " << log << std::endl;
    }

    this->ID = shaderProgram;

    // Cleanup
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    glDeleteShader(gShader);
}

/*
Shader::~Shader() {
    std::cout << "Destroying shader with ID " << ID << std::endl;
    glDeleteProgram(ID);
}
*/


void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    checkShaderActive();
    int uniformLocation = glGetUniformLocation(ID, name.c_str());
    glUniform1i(uniformLocation, (int)value);
}

void Shader::setBool(const GLint loc, bool value) const {
    checkShaderActive();
    glUniform1i(loc, (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    checkShaderActive();
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const GLint loc, int value) const {
    checkShaderActive();
    glUniform1i(loc, value);
}

void Shader::setFloat(const std::string& name, float value) const {
    checkShaderActive();
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const GLint loc, float value) const {
    checkShaderActive();
    glUniform1f(loc, value);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    checkShaderActive();
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const GLint loc, float x, float y, float z) const {
    checkShaderActive();
    glUniform3f(loc, x, y, z);
}

void Shader::setVec3(const std::string &name, glm::vec3 v) const {
    checkShaderActive();
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::setVec3(const GLint loc, glm::vec3 v) const {
    checkShaderActive();
    glUniform3fv(loc, 1, glm::value_ptr(v));
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
    checkShaderActive();
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const GLint loc, glm::mat4 value) const {
    checkShaderActive();
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat3(const std::string &name, glm::mat3 value) const {
    checkShaderActive();
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat3(const GLint loc, glm::mat3 value) const {
    checkShaderActive();
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::checkShaderActive() const {
    int programId;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programId);
    if (programId != ID) {
        std::cerr << "Shader [" << ID << "] is not in use, cannot set uniform on it" << std::endl;
    }
}
