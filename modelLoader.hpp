#ifndef MODEL_H
#define MODEL_H

#include "newMesh.hpp"

class ModelLoader {
public:
    ModelLoader();
    
    std::vector<Mesh> loadMesh(const std::string &path, Material &material);
private:
};

#endif
