#pragma once

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>

#include "tinygltf/tiny_gltf.h"

#include <string>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Program.h"
#include "Transform.h"

struct Texture
{
    GLuint id;
    std::string type; // example: "texture_diffuse", "texture_normal", "texture_specular"
    std::string path; // we store the path of the texture to compare with other textures
};

struct Mesh
{
    GLuint vao;
    std::map<int, GLuint> vbos;
    uint materialId;
};

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class Model
{
    public:
        // Model() {};
        // ~Model();

        void loadGLTF(std::string filepath);
        void draw(const Program &program);

    private:
        Mesh bindMesh(tinygltf::Model &model, tinygltf::Mesh &mesh);
        void bindModelNodes(tinygltf::Model &model, tinygltf::Node &node);
        void loadTextures();
        void bindModel();

        void drawMesh(const Mesh& glMesh, tinygltf::Model& model, tinygltf::Mesh& mesh);
        void drawModelNodes(tinygltf::Model& model, tinygltf::Node& node, glm::mat4 nodeMatrix);
        void drawModel(tinygltf::Model& model);

    public:
        Transform transform;

    private:
        tinygltf::Model _model;
        std::map<int, Mesh> _meshes;
        std::map<int, Texture> _textures;

        const Program* _cachedProgram;
};
