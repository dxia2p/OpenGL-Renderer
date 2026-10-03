#include "modelLoader.hpp"

#include <algorithm>
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
unsigned int textureFromFile(const std::filesystem::path &path, bool srgb) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, numChannels;
    unsigned char *data = stbi_load(path.string().c_str(), &width, &height, &numChannels, 0);
    // Check for errors loading data
    if (!data) {
        std::cerr << "Error loading texture at path: " << path.string() << std::endl;
        return 0;
    }
    //
    // Decide what format the image is in 
    GLenum format;
    GLenum internalFormat;
    switch (numChannels) {
        case 1:
            format = GL_R;
            internalFormat = GL_R;
            break;
        case 2:
            format = GL_RG;
            internalFormat = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = srgb ? GL_SRGB : GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;
            break;
        default:
            std::cerr << "Invalid number of channels (" << numChannels << ") in textureFromFile()" << std::endl;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return texture;
}

// const std::vector<Vertex> cubeVertices{
//     // Right face
//     Vertex(glm::vec3(0.5, 0.5, 0.5), glm::vec3(1.0, 0, 0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, 0.5, -0.5), glm::vec3(1.0, 0, 0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, -0.5, 0.5), glm::vec3(1.0, 0, 0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, -0.5, -0.5), glm::vec3(1.0, 0, 0), glm::vec2(0, 0)),

//     // Left face
//     Vertex(glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0, 0)),

//     // Bottom face
//     Vertex(glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0, 0)),  
//     Vertex(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0, 0)),

//     // Top face
//     Vertex(glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0, 0)),    

//     // Front face
//     Vertex(glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0, 0)),
    
//     // Back face
//     Vertex(glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0, 0)),
//     Vertex(glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0, 0)),
// };

// const std::vector<unsigned int> cubeIndices{
//     // Right face
//     2, 3, 1,
//     2, 1, 0,

//     // Left face
//     6, 5, 7,
//     6, 4, 5,

//     // Bottom face
//     8, 11, 9,
//     8, 10, 11,

//     // Top face
//     12, 13, 15,
//     12, 15, 14,

//     // Front face
//     16, 17, 19,
//     16, 19, 18,
    
//     // Back face
//     20, 22, 23,
//     20, 23, 21
// };
}  // namespace


// Mesh ModelLoader::loadCube(Shader *defaultShader) {
//     return Mesh(glm::vec3(0), std::make_shared<Material>(), cubeVertices, cubeIndices);
// }


std::vector<Mesh> ModelLoader::load(const std::filesystem::path &path, Shader *defaultShader, bool flipUVs) {
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate |  aiProcess_PreTransformVertices;
    if (flipUVs) flags |= aiProcess_FlipUVs;
    const aiScene *scene = importer.ReadFile(path.string(), flags);
    if (scene == nullptr) {
        std::cerr << "Error loading model at: " << path.string() << std::endl;
        return std::vector<Mesh>();
    }

    // Set currentDirectory
    currentDirectory = path.parent_path();
    
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
        
        std::string rawTexturePath = str.C_Str();
        // Replace Windows backslashes with forward slashes for cross-platform safety
        std::replace(rawTexturePath.begin(), rawTexturePath.end(), '\\', '/');

        std::filesystem::path texP(rawTexturePath);
        std::filesystem::path texturePath;

        if (texP.is_absolute() && std::filesystem::exists(texP)) {
            texturePath = texP;
        } else if (texP.is_absolute()) {
            // If absolute path from an external machine doesn't exist, try relative to currentDirectory
            texturePath = currentDirectory / texP.filename();
        } else {
            // Relative path: remove any leading slashes
            while (!rawTexturePath.empty() && (rawTexturePath.front() == '/' || rawTexturePath.front() == '\\')) {
                rawTexturePath.erase(rawTexturePath.begin());
            }
            texturePath = (currentDirectory / std::filesystem::path(rawTexturePath)).lexically_normal();
        }

        // If the file doesn't exist at texturePath, check if the filename exists in currentDirectory or a textures subdirectory
        if (!std::filesystem::exists(texturePath)) {
            if (std::filesystem::exists(currentDirectory / texP.filename())) {
                texturePath = currentDirectory / texP.filename();
            } else if (std::filesystem::exists(currentDirectory / "textures" / texP.filename())) {
                texturePath = currentDirectory / "textures" / texP.filename();
            }
        }

        auto it = loadedTextures.find(texturePath); 
        if (it != loadedTextures.end()) {
            result = it->second;
        } else {
            result.id = textureFromFile(texturePath, internalType == TextureType::Diffuse ? true : false);  // TODO: Change this to handle more texture types
            result.textureType = internalType;
            result.path = texturePath;
            loadedTextures[texturePath] = result;
        }

    } else {
        result.textureType = TextureType::None;
    }
    return result;
}
