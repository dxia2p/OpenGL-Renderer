#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>
#include "shader.hpp"

/* 
The mesh class handles the creation of vertex and element buffers from a list of vertices and indices.
It also acts as a container for these buffers along with ID's of textures that have already been loaded into OpenGL.
NOTE: All shaders must follow the convention of "texture_<diffuse/specular><number>" when naming their sampler2D's
*/

constexpr int TEXTURE_TYPES_SIZE = 2;
enum class TextureTypes {
    Diffuse = 0,
    Specular
};

std::string getShaderTextureUniformName(TextureTypes type, int index);

struct Texture {
    unsigned int id;
    TextureTypes textureType;
    std::string path;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures);
    void draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) const;

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    std::vector<Texture> textures;
};

#endif
