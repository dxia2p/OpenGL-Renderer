#include "modelLoader.hpp"

#include <assimp/material.h>
#include <iostream>
#include <iterator>
#include <memory>

#include "material.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glad/glad.h"

namespace {
// Helper function for loading a texture from a file and sending it to the gpu
unsigned int textureFromFile(std::string path, GLint format) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, numChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
    // Check for errors loading data
    if (!data) {
        std::cerr << "Error loading texture at path: " << path << std::endl;
        return 0;
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return texture;
}
}


std::vector<Mesh> ModelLoader::load(const std::string &path, Shader *defaultShader) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (scene == nullptr) {
        std::cerr << "Error loading model at: " + path << std::endl;
        return std::vector<Mesh>();
    }

    // Set currentDirectory
    size_t lastSlash = path.find_last_of('/');
    currentDirectory = path.substr(0, lastSlash + 1);  // Add one to lastSlash to keep the slash at the end
    
    this->defaultShader = defaultShader;

    std::vector<Mesh> meshes;
    processNode(scene->mRootNode, scene, meshes);
    return meshes;
}

void ModelLoader::processNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes) {
    // Process each mesh in the node
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];  // node->mMeshes[i] is a list of indices to the scene's meshes
        meshes.push_back(processMesh(mesh, scene));
    }

    // Recursively process children of this node
    for(int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, meshes);
    }
}

Mesh ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> meshMaterial;
    // Process vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
        Vertex vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            // Note that a mesh can have more than 1 set of texure coordinates
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.texCoords = glm::vec2(0);
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process material
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // Load textures
        Texture diffuseMap = loadTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
        Texture specularMap = loadTextures(material, aiTextureType_SPECULAR, TextureType::Specular);
        // Make material for the mesh
        meshMaterial = std::make_shared<Material>();
        meshMaterial->setDiffuseTexture(diffuseMap);
        meshMaterial->setSpecularTexture(specularMap);
        meshMaterial->color = glm::vec3(1);
        meshMaterial->shader = defaultShader;
        meshMaterial->shininess = 32;  // TODO: Change this
    }

    return Mesh(glm::vec3(0), meshMaterial, vertices, indices);
}


Texture ModelLoader::loadTextures(aiMaterial *mat, aiTextureType aiType, TextureType internalType) {
    Texture result;
    if (mat->GetTextureCount(aiType) > 0) {
        // This function only gets the first texture of the given type in the material (for now)
        aiString str;
        mat->GetTexture(aiType, 0, &str);  // GetTexture puts relative path into str (most of the time)
        std::string textureDir = currentDirectory + str.C_Str();
        std::unordered_map<std::string, Texture>::iterator it = loadedTextures.find(textureDir); 
        if (it != loadedTextures.end()) {
            result = loadedTextures[textureDir];
        } else {
            result.id = textureFromFile(textureDir, (aiType == aiTextureType_DIFFUSE) ? GL_RGB : GL_R);  // TODO: Change this to handle more texture types
            result.textureType = internalType;
            result.path = textureDir;
            loadedTextures[textureDir] = result;
        }

    } else {
        result.textureType = TextureType::None;
    }
    return result;
}
