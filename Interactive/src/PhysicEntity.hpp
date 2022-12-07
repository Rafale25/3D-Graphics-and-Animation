#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Transform.h"

class PhysicEntity
{
    public:
        PhysicEntity(Transform* t, float radius):
        transform(t),
        _radius(radius)
        {
        }
        // ~PhysicEntity();

        void update(float dt)
        {
            if (!_active) return;

            const float gravity = 0.01f;

            _velocity *= 0.99f;
            transform->position += _velocity;

            if (transform->position.y - _radius < 0.0f) {
                transform->position.y = _radius;

                if (glm::length(_velocity.y) > 0.03f) {
                    glm::vec3 reflectedVelocity = glm::reflect(_velocity, glm::vec3(0.0f, 1.0f, 0.0f));
                    const float bounce = 0.5f;
                    _velocity = reflectedVelocity * bounce;
                } else {
                    _velocity.y = 0.0f;
                    // _velocity = glm::vec3(0.0f);
                }

                const float groundFriction = 0.95f;
                _velocity.x *= groundFriction;
                _velocity.y *= groundFriction;

            } else {
                _velocity.y -= gravity;
            }
        }

        void setActive(bool value)
        {
            _active = value;
            if (value == false) {
                _velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            }
        }

    public:
        Transform* transform;
        glm::vec3 _velocity = glm::vec3(0.0f, 0.0f, 0.0f);

    private:
        bool _active = true;
        float _radius = 1.0f;
};
