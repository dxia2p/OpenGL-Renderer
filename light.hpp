#include <glm/glm.hpp>

class Light {
public:
    virtual ~Light() = default;

    virtual void setAmbient(glm::vec3 val) {
        // DO STUFF
    };
    virtual void setDiffuse(glm::vec3 val) {

    }
    virtual void setSpecular(glm::vec3 val) {

    }

protected:
    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : ambient(ambient), diffuse(diffuse), specular(specular) {}

private:
    glm::vec3 ambient, diffuse, specular;
};


class DirectionalLight : public Light {
public:
    DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction) : Light(ambient, diffuse, specular), direction(direction) {}
private:
    glm::vec3 direction;
};
