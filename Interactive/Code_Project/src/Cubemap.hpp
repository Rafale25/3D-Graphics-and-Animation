#pragma once

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>

#include <string>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.h"

class Cubemap
{
    public:
        // Cubemap();
        // ~Cubemap();
        void load()
        {

        }

    private:
        GLuint _vao;
        Shader _program;
};

// Cubemap::Cubemap(){}
// Cubemap::~Cubemap(){}
