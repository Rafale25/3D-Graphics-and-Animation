#pragma once

#include <iostream>
#include <vector>

#include "View.hpp"
#include "shader.h"
#include "Camera.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
// #include <glm/gtc/random.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include "tinygltf/stb_image.h"

#include "Model.hpp"

class BloatView: public View {
    public:
        BloatView(Context& ctx): View(ctx) {

            int width, height;
            glfwGetWindowSize(ctx.window, &width, &height);

            camera = OrbitCamera(
                glm::vec3(0.0f),
                0.0f, 0.0f, 5.0f,
                60.0f, (float)width / (float)height, 0.01f, 1000.0f
            );
            // camera = FPSCamera()

            _program_skybox = new Shader("./assets/shaders/skybox.vert", "./assets/shaders/skybox.frag");
            _program_model = new Shader("./assets/shaders/object.vs", "./assets/shaders/object.fs");

            Model model_maxwell = Model();
            model_maxwell.loadGLTF("assets/models/maxwell_the_cat/scene.gltf");
            model_maxwell.transform.scale *= 0.1f;
            model_maxwell.transform.position.x = -2.0f;
            model_maxwell.transform.position.z = 2.0f;
            model_maxwell.transform.position.y = -1.0f;

            Model model_helmet = Model();
            model_helmet.loadGLTF("../../../Downloads/glTF-Sample-Models-master/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");

            _models.push_back(model_helmet);
            _models.push_back(model_maxwell);

            float skyboxVertices[] = {
                // positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
            };

            // skybox
            // unsigned int skyboxVAO, skyboxVBO;
            glGenVertexArrays(1, &_skyboxVAO);
            glGenBuffers(1, &_skyboxVBO);
            glBindVertexArray(_skyboxVAO);
            glBindBuffer(GL_ARRAY_BUFFER, _skyboxVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            // load textures
            // -------------
            std::vector<std::string> faces
            {
                "assets/textures/skybox/right.jpg",
                "assets/textures/skybox/left.jpg",
                "assets/textures/skybox/top.jpg",
                "assets/textures/skybox/bottom.jpg",
                "assets/textures/skybox/front.jpg",
                "assets/textures/skybox/back.jpg",
            };

            _cubemapTexture = loadCubemap(faces);
        }
        // ~BloatView() {}

        // loads a cubemap texture from 6 individual texture faces
        // order:
        // +X (right)
        // -X (left)
        // +Y (top)
        // -Y (bottom)
        // +Z (front)
        // -Z (back)
        // -------------------------------------------------------
        GLuint loadCubemap(std::vector<std::string> faces)
        {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

            int width, height, nrComponents;
            for (unsigned int i = 0; i < faces.size(); i++)
            {
                unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
                if (data)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    stbi_image_free(data);
                }
                else
                {
                    std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                    stbi_image_free(data);
                }
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            return textureID;
        }

        void onUpdate(float time_since_start, float dt)
        {
        }

        void onDraw(float time_since_start, float dt)
        {
            glEnable(GL_CULL_FACE);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (_wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // enable wireframe
            }

            _program_model->use();

            _program_model->setMat4("view_matrix", camera.getView());
            _program_model->setMat4("proj_matrix", camera.getProjection());

            _program_model->setVec3("view_pos", camera.getPosition());
            _program_model->setVec3("light_positions[0]", camera.getPosition());
            _program_model->setVec3("light_colors[0]", glm::vec3(1.0, 1.0, 1.0));

            glActiveTexture(GL_TEXTURE0 + 5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemapTexture);
            _program_model->setInt("skybox", 5);

            for (Model& model : _models) {
                model.draw(*_program_model);
            }

            // draw skybox as last
            glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
            _program_skybox->use();
            _program_skybox->setMat4("view_matrix", glm::mat4(glm::mat3(camera.getView()))); // remove translation from the view matrix
            _program_skybox->setMat4("proj_matrix", camera.getProjection());
            // skybox cube
            glBindVertexArray(_skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // set depth function back to default

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // disable wireframe

            ctx.imguiNewFrame();
            ImGui::Begin("Debug");

            ImGui::Text("%.4f ms", dt);
            ImGui::Text("%.2f fps", 1.0f / dt);

            ImGui::Spacing();

            ImGui::Checkbox("Wireframe", &_wireframe);

            ImGui::End();
            ctx.imguiRender();
        }

        void onMouseDrag(int x, int y, int dx, int dy)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureMouse)
                return;

            camera.onMouseDrag(x, y, dx, dy);
        }

        void onMouseScroll(int scroll_x, int scroll_y)
        {
            camera.onMouseScroll(scroll_x, scroll_y);
        }

        void onResize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }

    private:
        OrbitCamera camera;

        bool _wireframe = false;

        Shader* _program_model;
        Shader* _program_skybox;

        GLuint _skyboxVAO;
        GLuint _skyboxVBO;

        GLuint _cubemapTexture;

        std::vector<Model> _models;
};
