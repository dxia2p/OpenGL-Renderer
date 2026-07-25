#ifndef H_SKYBOX
#define H_SKYBOX

#include "shader.hpp"
#include <string>
#include <vector>

constexpr int SKYBOX_VERT_COUNT = 36;

class Skybox {
public:
    Skybox(std::vector<std::string> facePaths, Shader *shader);

    Shader *shader;
    
    unsigned int getVAO() { return VAO; }
    unsigned int getCubemap() { return cubemapID; }
private:
    unsigned int cubemapID, VAO, VBO;
};


#endif
