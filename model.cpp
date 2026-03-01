#include "model.hpp"

#include <iostream>
#include <glad/glad.h>
#include <assimp/postprocess.h>

// ------------------------------------------------ Prototypes ------------------------------------------------

static unsigned int TextureFromFile(std::string path);


// ------------------------------------------------ Model ------------------------------------------------

Model::Model(const std::string &path) {
    importModel(path);
    defaultMaterial = 
}

void Model::importModel(const std::string &filePath) {
    // We use assimp to import the model
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    // Report errors
    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Model::importModel failed when reading from file at" << filePath << std::endl;
        return;
    }

    directory = filePath.substr(0, filePath.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // Process each mesh pointed to by the node
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // Do the same for all other nodes
    for(int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices, normals and UVs
    for(int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {  // only get the first set of texture coords for now
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices
    
    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process material
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureTypes::Diffuse);

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureTypes::Specular);

        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureTypes typeName) {
    std::vector<Texture> textures;
    for(int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string path = directory + str.C_Str();
        
        // Check if the texture has already been loaded, if it has then simply add it to textures
        // Without this optimization the same textures will be loaded many times
        auto it = texturesLoaded.find(path);
        if (it != texturesLoaded.end()) {
            textures.push_back(it->second);
        } else {
            Texture texture;
            texture.path = path;
            texture.id = TextureFromFile(texture.path);
            texture.textureType = typeName;
            textures.push_back(texture);
            texturesLoaded[path] = texture;
        }

    }
    return textures;
}

// Make sure the image is 8 bit color depth!
static unsigned int TextureFromFile(std::string path) {
    unsigned int id;
    int width, height, numChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);

    if (data) {
        GLenum format;
        switch (numChannels) {
            case 1:
                format = GL_RED;
                break;
            case 2:
                std::cerr << "Model::TextureFromFile does not support images with 2 channels" << std::endl;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
        }

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cerr << "Model::TextureFromFile failed to load texture at " << path << std::endl;
        return 0;
    }

    return id;
}

// ------------------------------------------------ Model Instance ------------------------------------------------
