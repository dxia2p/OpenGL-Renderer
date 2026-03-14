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

/*
 * TODO: Refactor model and mesh classes into a ModelLoader class and a Mesh class  
 * The Mesh class will store position, rotation, scale, vertices, indices, and a material
 * The ModelLoader class will have a function to load a model from a filepath and return a vector of Meshes
 * loader function will also take a default shader which it will store in the material of each returned mesh
 * */


class Model {
public:
    Model(const std::string &path, Shader *defaultShader);

    const std::vector<Material> &getDefaultMaterials() const { return defaultMaterials; }
    const std::map<int, std::vector<Mesh>> &getMeshes() const { return meshes; }
private:
    std::map<int, std::vector<Mesh>> meshes;  // Key is index of material in defaultMaterials
    std::vector<Material> defaultMaterials;
    std::string directory;
    std::map<std::string, Texture> texturesLoaded;
    Shader *defaultShader;  // Put this into Material when model is initialized

    void importModel(const std::string &filePath);
    void processNode(aiNode *node, const aiScene *scene);
    std::pair<int, Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureTypes typeName);
};


class ModelInstance {
public:
    glm::vec3 position, scale;
    glm::quat rotation = glm::quat(glm::vec3(0));

    ModelInstance(Model *model, glm::vec3 position = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1, 1, 1)) : model(model), position(position), rotation(glm::quat(rotation)), scale(scale) {}

    const Model* getModel() const { return model; }
private:
    Model *model;
};


#endif
