#pragma once

#include <iostream>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <tinygltf/stb_image.h>

#include "View.hpp"
#include "shader.h"
#include "Camera.hpp"
#include "Model.hpp"
#include "Skybox.hpp"

// #include <glm/gtc/random.hpp>
// #include <glm/gtc/matrix_transform.hpp>

class BloatView: public View {
    public:
        BloatView(Context& ctx): View(ctx) {

            int width, height;
            glfwGetWindowSize(ctx.window, &width, &height);
            glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // _camera = OrbitCamera(
            //     glm::vec3(0.0f),
            //     0.0f, 0.0f, 5.0f,
            //     60.0f, (float)width / (float)height, 0.01f, 1000.0f
            // );
            _camera = FPSCamera(
                glm::vec3(0.0f),
                60.0f, (float)width / (float)height, 0.01f, 1000.0f
            );

            _program_skybox = new Shader("./assets/shaders/skybox.vert", "./assets/shaders/skybox.frag");
            _program_model = new Shader("./assets/shaders/object.vs", "./assets/shaders/object.fs");

            Model model_maxwell = Model();
            model_maxwell.loadGLTF("assets/models/maxwell_the_cat/scene.gltf");
            model_maxwell.transform.scale *= 0.1f;
            model_maxwell.transform.position.x = -2.0f;
            model_maxwell.transform.position.z = 2.0f;
            model_maxwell.transform.position.y = -1.0f;

            Model model_helmet = Model();
            model_helmet.loadGLTF("assets/DamagedHelmet/glTF/DamagedHelmet.gltf");

            _models.push_back(model_helmet);
            _models.push_back(model_maxwell);
        }
        // ~BloatView() {}

        void onUpdate(float time_since_start, float dt)
        {
            float x = keyState[GLFW_KEY_A] - keyState[GLFW_KEY_D];
            float y = keyState[GLFW_KEY_LEFT_CONTROL] - keyState[GLFW_KEY_SPACE];
            float z = keyState[GLFW_KEY_W] - keyState[GLFW_KEY_S];
            _camera.move(glm::vec3(x, y, z));

            _camera.update();
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

            _program_model->setMat4("view_matrix", _camera.getView());
            _program_model->setMat4("proj_matrix", _camera.getProjection());

            _program_model->setVec3("view_pos", _camera.getPosition());
            _program_model->setVec3("light_positions[0]", _camera.getPosition());
            _program_model->setVec3("light_colors[0]", glm::vec3(1.0, 1.0, 1.0));

            glActiveTexture(GL_TEXTURE0 + 5);
            // glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemapTexture);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox.cubemapTexture);
            _program_model->setInt("skybox", 5);

            for (Model& model : _models) {
                model.draw(*_program_model);
            }

            // draw skybox as last
            _program_skybox->use();
            _program_skybox->setMat4("view_matrix", glm::mat4(glm::mat3(_camera.getView()))); // remove translation from the view matrix
            _program_skybox->setMat4("proj_matrix", _camera.getProjection());
            _skybox.draw();

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

        void onKeyPress(int key)
        {
            keyState[key] = true;

            if (key == GLFW_KEY_C) {
                _cursorEnable = !_cursorEnable;

                if (_cursorEnable)
                    glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                else
                    glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }

        void onKeyRelease(int key)
        {
            keyState[key] = false;
        }


        void onMouseDrag(int x, int y, int dx, int dy)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureMouse)
                return;

            // _camera.onMouseDrag(x, y, dx, dy);
        }

        void onMouseScroll(int scroll_x, int scroll_y)
        {
            // _camera.onMouseScroll(scroll_x, scroll_y);
        }

        void onMouseMotion(int x, int y, int dx, int dy)
        {
            if (!_cursorEnable)
                _camera.onMouseMotion(x, y, dx, dy);
        }

        void onResize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }

    private:
        int keyState[GLFW_KEY_LAST] = {0};

        // OrbitCamera _camera;
        FPSCamera _camera;

        bool _wireframe = false;
        bool _cursorEnable = false;

        Shader* _program_model;
        Shader* _program_skybox;

        Skybox _skybox;

        std::vector<Model> _models;
};
