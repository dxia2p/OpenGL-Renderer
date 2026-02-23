#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <map>
#include "mesh.hpp"

class Model {
public:
    glm::vec3 position = glm::vec3(0), scale = glm::vec3(1);
    glm::quat rotation;

    Model(const std::string &filePath);
    void draw(Shader &shader, glm::mat4 view, glm::mat4 projection) const;
private:
    std::vector<Mesh> meshes;
    std::map<std::string, Texture> texturesLoaded;
    std::string directory;

    void importModel(const std::string &filePath);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureTypes typeName);
};

#endif
