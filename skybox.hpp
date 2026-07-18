#ifndef H_SKYBOX
#define H_SKYBOX

#include "shader.hpp"
#include <string>
#include <vector>

class Skybox {
public:
    Skybox(std::vector<std::string> facePaths, Shader *shader);
private:
    unsigned int cubemapID, VAO, VBO;
    Shader *shader;
};


#endif
