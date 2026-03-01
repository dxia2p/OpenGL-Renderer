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
#include "material.hpp"
#include "mesh.hpp"

class Model {
public:
    Model(const std::string &path);

    std::vector<Material> getDefaultMaterials() { return defaultMaterials; }
private:
    std::vector<Mesh> meshes;
    std::vector<Material> defaultMaterials;
    std::string directory;
    std::map<std::string, Texture> texturesLoaded;

    void importModel(const std::string &filePath);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureTypes typeName);
};


class ModelInstance {
public:
    glm::vec3 position, scale;
    glm::quat rotation = glm::quat(glm::vec3(0));

    ModelInstance(Model *model, glm::vec3 position = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1, 1, 1)) : model(model), position(position), rotation(glm::quat(rotation)), scale(scale) {}

    void draw(Shader &shader, glm::mat4 view, glm::mat4 projection) const;
private:
    Model *model;
};


#endif
