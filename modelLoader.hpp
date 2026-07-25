#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <map>
#include <string>

#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelLoader {
public:
    ModelLoader() {}
    
    std::vector<Mesh> load(const std::string &path, Shader *defaultShader);
private:
    std::unordered_map<std::string, Texture> loadedTextures;
    std::string currentDirectory;  // currentDirectory includes a slash at the end
    Shader *defaultShader;  // Stores a default shader we put into created materials

    void processNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    Texture loadTextures(aiMaterial *mat, aiTextureType aiType, TextureType internalType);
};

#endif
