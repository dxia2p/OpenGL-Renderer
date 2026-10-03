#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <filesystem>
#include <map>
#include <string>

#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelLoader {
public:
    ModelLoader() {}
    
    std::vector<Mesh> load(const std::filesystem::path &path, Shader *defaultShader, bool flipUVs);
    Mesh loadCube(Shader *defaultShader);
private:
    std::map<std::filesystem::path, Texture> loadedTextures;
    std::filesystem::path currentDirectory;
    Shader *defaultShader;  // Stores a default shader we put into created materials

    void processNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    Texture loadTextures(aiMaterial *mat, aiTextureType aiType, TextureType internalType);
};

#endif
