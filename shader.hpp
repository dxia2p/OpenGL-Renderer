#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    //~Shader();
    
    // Activate the shader
    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec3(const std::string &name, glm::vec3 v) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setMat3(const std::string &name, glm::mat3 value) const;
};


#endif
