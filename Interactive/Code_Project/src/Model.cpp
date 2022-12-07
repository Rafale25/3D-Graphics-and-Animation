#include "Model.hpp"

void Model::loadGLTF(std::string filepath)
{
    std::cout << "Trying to load model " << filepath << "\n";

    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&_model, &err, &warn, filepath);
    if (!warn.empty()) std::cout << "WARNING - GLTF: " << warn << std::endl;
    if (!err.empty())  std::cout << "ERROR - GLTF: " << err << std::endl;

    if (!res) {
        std::cout << "ERROR - Failed to load glTF: " << filepath << std::endl;
        throw new std::runtime_error("Model::loadGLTF failed.");
    }
    else
    {
        std::cout << "OK - Loaded glTF: " << filepath << std::endl;
    }

    bindModel();
}

void Model::draw(const Shader &program)
{
    _cachedProgram = &program;
    drawModel(_model);
}

Mesh Model::bindMesh(tinygltf::Model &model, tinygltf::Mesh &mesh)
{
    Mesh glMesh;
    glGenVertexArrays(1, &glMesh.vao);
    glBindVertexArray(glMesh.vao);

    for (size_t i = 0; i < model.bufferViews.size(); ++i)
    {
        const tinygltf::BufferView &bufferView = model.bufferViews[i];
        if (bufferView.target == 0)
        { // TODO impl drawarrays
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue; // Unsupported bufferView.
                    /*
                        From spec2.0 readme:
                        https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                                ... drawArrays function should be used with a count equal to
                        the count            property of any of the accessors referenced by the
                        attributes            property            (they are all equal for a given
                        primitive).
                    */
        }

        const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
        // std::cout << "bufferview.target " << bufferView.target << std::endl;

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glMesh.vbos[i] = vbo;
        glBindBuffer(bufferView.target, vbo);

        // std::cout << "buffer.data.size = " << buffer.data.size()
        //         << ", bufferview.byteOffset = " << bufferView.byteOffset
        //         << std::endl;

        glBufferData(bufferView.target, bufferView.byteLength,
                    &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        for (auto &attrib : primitive.attributes)
        {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, glMesh.vbos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR)
            {
                size = accessor.type;
            }

            int vaa = -1; // vertex array attribute
            if (attrib.first.compare("POSITION") == 0)
                vaa = 0;
            if (attrib.first.compare("NORMAL") == 0)
                vaa = 1;
            // if (attrib.first.compare("TANGENT") == 0)
            //     vaa = 2;
            if (attrib.first.compare("TEXCOORD_0") == 0)
                vaa = 2;
            if (vaa > -1)
            {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                    accessor.normalized ? GL_TRUE : GL_FALSE,
                                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
            }
            else
                std::cout << "vaa missing: " << attrib.first << std::endl;
        }
    }

    return glMesh;
}

void Model::bindModelNodes(tinygltf::Model &model, tinygltf::Node &node)
{
    if ((node.mesh >= 0) && ((size_t)node.mesh < model.meshes.size()))
    {
        Mesh mesh = bindMesh(model, model.meshes[node.mesh]);
        _meshes[node.mesh] = mesh;
    }

    for (size_t i = 0; i < node.children.size(); ++i)
    {
        assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
        bindModelNodes(model, model.nodes[node.children[i]]);
    }
}

static bool stringContainsAny(std::string str, const std::vector<std::string> substrings)
{
    for (uint i = 0 ; i < str.length() ; ++i)
        str[i] = tolower(str[i]);

    for (std::string s : substrings) {
        if (str.find(s) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

void Model::loadTextures()
{
    for (size_t i = 0; i < _model.textures.size(); ++i)
    {
        tinygltf::Texture &texture = _model.textures[i];

        if (texture.source > -1)
        {
            GLuint textureId;
            glGenTextures(1, &textureId);

            tinygltf::Image &image = _model.images[texture.source];

            glBindTexture(GL_TEXTURE_2D, textureId);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            GLenum format = GL_RGBA;

            if (image.component == 1) format = GL_RED;
            else if (image.component == 2) format = GL_RG;
            else if (image.component == 3) format = GL_RGB;
            else { // ???
            }

            GLenum type = GL_UNSIGNED_BYTE;
            if (image.bits == 8){// ok
            }
            else if (image.bits == 16) type = GL_UNSIGNED_SHORT;
            else { // ???
            }

            const std::vector<std::string> diffuseKeywords = {"albedo", "diffuse", "basecolor", "color", "colors"};
            const std::vector<std::string> normalsKeywords = {"normal", "normals", "bump"};
            const std::vector<std::string> metallicRoughnessKeywords = {"roughness", "metallic", "metal", "rough"};
            const std::vector<std::string> ambiantOcclusionKeywords = {"ao", "ambientocclusion"};
            const std::vector<std::string> emissiveKeywords = {"emissive", "emission"};

            std::string textureType;

            if (stringContainsAny(image.uri, diffuseKeywords))
            {
                textureType = "texture_diffuse";
            }
            else if (stringContainsAny(image.uri, normalsKeywords))
            {
                textureType = "texture_normal";
            }
            else if (stringContainsAny(image.uri, metallicRoughnessKeywords))
            {
                textureType = "texture_metallic_roughness";
            }
            else if (stringContainsAny(image.uri, ambiantOcclusionKeywords))
            {
                textureType = "texture_ambiant_occlusion";
            }
            else if (stringContainsAny(image.uri, emissiveKeywords))
            {
                textureType = "texture_emission";
            }
            else
            {
                std::cout << image.uri << " texture: Texture name doesn't contain keyword, defaulting to diffuse" << std::endl;
                continue;
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                        format, type, &image.image.at(0));
            std::cout << image.uri << " is used as " << textureType << std::endl;

            _textures[i] = Texture{textureId, textureType, image.uri};
        }
    }
}

void Model::bindModel()
{
    const tinygltf::Scene &scene = _model.scenes[_model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i)
    {
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < _model.nodes.size()));
        bindModelNodes(_model, _model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);

    // cleanup vbos but do not delete index buffers yet
    // for (auto it = vbos.cbegin(); it != vbos.cend();)
    // {
    //     tinygltf::BufferView bufferView = model.bufferViews[it->first];
    //     if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER)
    //     {
    //         glDeleteBuffers(1, &vbos[it->first]);
    //         vbos.erase(it++);
    //     }
    //     else
    //     {
    //         ++it;
    //     }
    // }

    loadTextures();
}

static glm::mat4 getNodeMatrix(const tinygltf::Node& node)
{
    glm::mat4 m = glm::mat4(1.0f);

    if (node.translation.size() > 0) {
        m = glm::translate(m, {node.translation[0], node.translation[1], node.translation[2]});
    }
    if (node.rotation.size() > 0) {
        m = glm::rotate(m, (float)node.rotation[0], {1.0f, 0.0f, 0.0f});
        m = glm::rotate(m, (float)node.rotation[1], {0.0f, 1.0f, 0.0f});
        m = glm::rotate(m, (float)node.rotation[2], {0.0f, 0.0f, 1.0f});
    }
    if (node.scale.size() > 0) {
        m = glm::scale(m, {node.scale[0], node.scale[1], node.scale[2]});
    }

    return m;
}

void Model::drawMesh(const Mesh& glMesh, tinygltf::Model& model, tinygltf::Mesh& mesh)
{
    glBindVertexArray(glMesh.vao);

    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        const tinygltf::Primitive& primitive = mesh.primitives[i];
        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
        const tinygltf::Material& material = model.materials[primitive.material];

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh.vbos.at(indexAccessor.bufferView));
        Texture texture;

        if (material.pbrMetallicRoughness.baseColorTexture.index > -1) {
            texture = _textures[material.pbrMetallicRoughness.baseColorTexture.index];
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(_cachedProgram->ID, texture.type.c_str()), 0);
            glBindTexture(GL_TEXTURE_2D, _textures[material.pbrMetallicRoughness.baseColorTexture.index].id);
        }

        if (material.normalTexture.index > -1) {
            texture = _textures[material.normalTexture.index];
            glActiveTexture(GL_TEXTURE0 + 1);
            glUniform1i(glGetUniformLocation(_cachedProgram->ID, texture.type.c_str()), 1);
            glBindTexture(GL_TEXTURE_2D, _textures[material.normalTexture.index].id);
        }

        if (material.pbrMetallicRoughness.metallicRoughnessTexture.index > -1) {
            texture = _textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index];
            glActiveTexture(GL_TEXTURE0 + 2);
            glUniform1i(glGetUniformLocation(_cachedProgram->ID, texture.type.c_str()), 2);
            glBindTexture(GL_TEXTURE_2D, _textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index].id);
        }

        if (material.occlusionTexture.index > -1) {
            texture = _textures[material.occlusionTexture.index];
            glActiveTexture(GL_TEXTURE0 + 3);
            glUniform1i(glGetUniformLocation(_cachedProgram->ID, texture.type.c_str()), 3);
            glBindTexture(GL_TEXTURE_2D, _textures[material.occlusionTexture.index].id);
        }

        if (material.emissiveTexture.index > -1) {
            texture = _textures[material.emissiveTexture.index];
            glActiveTexture(GL_TEXTURE0 + 4);
            glUniform1i(glGetUniformLocation(_cachedProgram->ID, texture.type.c_str()), 4);
            glBindTexture(GL_TEXTURE_2D, _textures[material.emissiveTexture.index].id);
        }

        if (material.alphaMode == "BLEND")
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);

        glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType, BUFFER_OFFSET(indexAccessor.byteOffset));
    }

    // unbind all textures
    for (int i = 0 ; i < 5 ; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(0);
}

void Model::drawModelNodes(tinygltf::Model& model, tinygltf::Node& node, glm::mat4 nodeMatrix)
{
    if ((node.mesh >= 0) && ((size_t)node.mesh < model.meshes.size()))
    {
        _cachedProgram->setMat4("model_matrix", nodeMatrix);
        drawMesh(_meshes[node.mesh], model, model.meshes[node.mesh]);
    }

    for (size_t i = 0; i < node.children.size(); i++)
    {
        glm::mat4 childMatrix = nodeMatrix * getNodeMatrix(model.nodes[node.children[i]]);
        drawModelNodes(model, model.nodes[node.children[i]], childMatrix);
    }
}

void Model::drawModel(tinygltf::Model& model)
{
    glm::mat4 modelMatrix = transform.getMatrix();

    const tinygltf::Scene &scene = model.scenes[model.defaultScene];
    for (uint i = 0; i < scene.nodes.size(); ++i)
    {
        glm::mat4 nodeMatrix = modelMatrix * getNodeMatrix(model.nodes[scene.nodes[i]]);
        drawModelNodes(model, model.nodes[scene.nodes[i]], nodeMatrix);
    }

    // glBindTexture(GL_TEXTURE_2D, 0);
    // glActiveTexture(GL_TEXTURE0);
}
