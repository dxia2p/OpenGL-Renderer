#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glm/glm.hpp>

#include <glad/glad.h>

class Shader {
public:
    unsigned int ID;

    Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath = "");
    //~Shader();
    
    // Activate the shader
    void use();

    void setBool(const std::string &name, bool value) const;
    void setBool(const GLint loc, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setInt(const GLint loc, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setFloat(const GLint loc, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec3(const GLint loc, float x, float y, float z) const;
    void setVec3(const std::string &name, glm::vec3 v) const;
    void setVec3(const GLint loc, glm::vec3 v) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setMat4(const GLint loc, glm::mat4 value) const;
    void setMat3(const std::string &name, glm::mat3 value) const;
    void setMat3(const GLint loc, glm::mat3 value) const;

private:
    // Checks if the shader is activated and outputs error message if it isn't
    // Meant to be used in setUniform functions
    void checkShaderActive() const;
};


#endif
