#pragma once

#include <glm/glm.hpp>

struct Transform
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 getMatrix(void)
    {
        glm::mat4 m = glm::mat4(1.0f);

        m = glm::translate(m, this->position);
        m = glm::rotate(m, this->rotation.x, {1.0f, 0.0f, 0.0f});
        m = glm::rotate(m, this->rotation.y, {0.0f, 1.0f, 0.0f});
        m = glm::rotate(m, this->rotation.z, {0.0f, 0.0f, 1.0f});
        m = glm::scale(m, this->scale);

        return m;
    }
};
