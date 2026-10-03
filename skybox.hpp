#ifndef H_SKYBOX
#define H_SKYBOX

#include "shader.hpp"
#include <filesystem>
#include <string>
#include <vector>

constexpr int SKYBOX_VERT_COUNT = 36;

class Skybox {
public:
    Skybox(const std::vector<std::filesystem::path> &facePaths, Shader *shader, bool hdr);
    Skybox(const std::vector<std::string> &facePaths, Shader *shader, bool hdr);

    Shader *shader;
    
    unsigned int getVAO() { return VAO; }
    unsigned int getCubemap() { return cubemapID; }
private:
    unsigned int cubemapID, VAO, VBO;
};


#endif
