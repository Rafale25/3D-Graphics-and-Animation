#pragma once

#include <iostream>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <tinygltf/stb_image.h>

#include "View.hpp"
#include "shader.h"
#include "Camera.hpp"
#include "Model.hpp"
#include "Skybox.hpp"
#include "FrameBuffer.hpp"
#include "PhysicEntity.hpp"

// #include <glm/gtc/random.hpp>
// #include <glm/gtc/matrix_transform.hpp>

class AppView: public View {
    public:
        AppView(Context& ctx): View(ctx) {

            glfwGetWindowSize(ctx.window, &_width, &_height);
            glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // _camera = OrbitCamera(
            //     glm::vec3(0.0f),
            //     0.0f, 0.0f, 5.0f,
            //     60.0f, (float)width / (float)height, 0.01f, 1000.0f
            // );
            _camera = FPSCamera(
                glm::vec3(5.0f, 3.0, 0.0f), 180.0f, 0.0f,
                // 60.0f, (float)_width / (float)_height, 0.01f, 1000.0f
                60.0f, (float)_width / (float)_height, 0.01f, 100.0f
            );

            _program_skybox = new Shader("./assets/shaders/skybox.vert", "./assets/shaders/skybox.frag");
            _program_model = new Shader("./assets/shaders/model_pbr.vs", "./assets/shaders/model_pbr.fs");
            _program_screen = new Shader("./assets/shaders/screen.vert", "./assets/shaders/screen.frag");

            _framebuffer.create(_width, _height);

            Model model_maxwell = Model();
            model_maxwell.loadGLTF("assets/models/maxwell_the_cat/scene.gltf");
            model_maxwell.transform.scale *= 0.1f;
            model_maxwell.transform.position.x = -2.0f;
            model_maxwell.transform.position.z = 2.0f;
            model_maxwell.transform.position.y = 10.0f;

            Model model_helmet = Model();
            model_helmet.loadGLTF("assets/models/DamagedHelmet/glTF/DamagedHelmet.gltf");

            Model model_robot = Model();
            model_robot.loadGLTF("assets/models/robot/robot.gltf");
            model_robot.transform.scale = glm::vec3(0.5f);
            model_robot.transform.position.z = -5.0f;

            Model model_floor = Model();
            model_floor.loadGLTF("assets/models/floor/plane.gltf");

            _models.push_back(model_helmet);
            _models.push_back(model_maxwell);
            _models.push_back(model_robot);
            _models.push_back(model_floor);

            // need to take from _models because they are created on the stack and it's making a copy
            PhysicEntity entity_helmet(&_models[0].transform, 1.0f);
            PhysicEntity entity_maxwell(&_models[1].transform, 0.0f);
            PhysicEntity entity_robot(&_models[2].transform, 1.0f);
            _physicEntities.insert(_physicEntities.end(),{
                entity_helmet,
                entity_robot,
                entity_maxwell,
            });

            int nbLights = 32;
            _program_model->use();
            _program_model->setInt("nb_lights", nbLights);
            for (int i = 0 ; i < nbLights ; ++i) {
                glm::vec3 p = glm::ballRand(15.0f);
                p.y *= 0.2f;
                p.y += 0.2f;

                _program_model->setVec3(
                    (std::string("light_positions[") + std::to_string(i) + std::string("]")).c_str(),
                    p
                );
                _program_model->setVec3(
                    (std::string("light_colors[") + std::to_string(i) + std::string("]")).c_str(),
                    glm::sphericalRand(1.0f)
                );
                _program_model->setFloat(
                    (std::string("light_intensities[") + std::to_string(i) + std::string("]")).c_str(),
                    3.0f
                );
            }

        }
        // ~BloatView() {}

        void onUpdate(float time_since_start, float dt)
        {
            float x = keyState[GLFW_KEY_A] - keyState[GLFW_KEY_D];
            float y = keyState[GLFW_KEY_LEFT_CONTROL] - keyState[GLFW_KEY_SPACE];
            float z = keyState[GLFW_KEY_W] - keyState[GLFW_KEY_S];
            _camera.move(glm::vec3(x, y, z));

            // holded entity
            if (_holdedEntity != nullptr) {
                _holdedEntity->transform->position = _camera.getPosition() + _camera.forward * _holdedDistance;
                _holdedEntity->transform->rotation = glm::quatLookAt(_camera.forward, glm::vec3(0.0f, 1.0f, 0.0f));
            }

            _camera.update(dt);

            static float fixedUpdateTime = 0.0f;
            fixedUpdateTime += dt;
            if (fixedUpdateTime > 1.0f/60.0f) {
                fixedUpdateTime = 0.0f;

                // fixed update code here (run 60 time per seconds)
                for (PhysicEntity& e : _physicEntities) {
                    e.update(dt);
                }
            }
        }

        void onDraw(float time_since_start, float dt)
        {
            glEnable(GL_CULL_FACE);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

            glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.framebuffer);

            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (_wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // enable wireframe
            }

            _program_model->use();

            _program_model->setMat4("view_matrix", _camera.getView());
            _program_model->setMat4("proj_matrix", _camera.getProjection());

            _program_model->setVec3("view_pos", _camera.getPosition());

            _program_model->setBool("enableSunlight", _sunLightEnable);
            _program_model->setBool("enablePointLights", _pointLightsEnable);

            _program_model->setVec3("sunDirection", glm::vec3(0.47, -.44, 0.77));
            _program_model->setFloat("sunIntensity", 12.0f);

            glActiveTexture(GL_TEXTURE0 + 5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox.cubemapTexture);
            _program_model->setInt("skybox", 5);

            for (Model& model : _models) {
                model.draw(*_program_model);
            }

            // draw skybox as last
            if (_skyboxEnable) {
                _program_skybox->use();
                _program_skybox->setMat4("view_matrix", glm::mat4(glm::mat3(_camera.getView()))); // remove translation from the view matrix
                _program_skybox->setMat4("proj_matrix", _camera.getProjection());
                _skybox.draw();
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // disable wireframe

            // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
            glDisable(GL_BLEND); // this or clear depth
            // clear all relevant buffers
            // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            _program_screen->use();
            _program_screen->setInt("texture_color", 0);
            _program_screen->setInt("texture_depth", 1);
            _program_screen->setVec2("resolution", glm::vec2(_width, _height));
            _program_screen->setInt("post_processing_id", _postProcessEffect);
            _framebuffer.draw();

            gui(time_since_start, dt);
        }

        void gui(float time_since_start, float dt)
        {
            ctx.imguiNewFrame();
            ImGui::Begin("Debug");

            ImGui::Text("%.4f ms", dt);
            ImGui::Text("%.2f fps", 1.0f / dt);

            ImGui::Text("%.2f %.2f %.2F", _camera.forward.x, _camera.forward.y, _camera.forward.z);

            ImGui::Spacing();

            ImGui::Checkbox("Wireframe", &_wireframe);
            ImGui::Checkbox("Skybox", &_skyboxEnable);

            ImGui::Checkbox("Sun light", &_sunLightEnable);
            ImGui::Checkbox("Point lights", &_pointLightsEnable);

            ImGui::InputInt("Post processing effect", &_postProcessEffect);
            if (_postProcessEffect < 0)
                _postProcessEffect = 0; // max-1
            // loop back when reach max

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

            if (key == GLFW_KEY_F) {
                if (_holdedEntity == nullptr) {

                    // catch an entity
                    for (PhysicEntity& e : _physicEntities) {
                        glm::vec3 p = _camera.getPosition() + _camera.forward * _holdedDistance;
                        if (glm::distance(p, e.transform->position) < 2.0f) {
                            e.setActive(false);
                            _holdedEntity = &e;
                        }
                    }

                } else {

                    // release an entity by setting it to null
                    _holdedEntity->setActive(true);
                    _holdedEntity->_velocity = _camera.forward * 0.2f;
                    _holdedEntity = nullptr;
                }
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
            _width = width;
            _height = height;
            _framebuffer.resize(width, height);
        }

    private:
        int keyState[GLFW_KEY_LAST] = {0};

        int _width, _height;

        // OrbitCamera _camera;
        FPSCamera _camera;

        bool _wireframe = false;
        bool _cursorEnable = false;

        bool _skyboxEnable = true;

        bool _sunLightEnable = true;
        bool _pointLightsEnable = true;

        int _postProcessEffect = 0;

        Shader* _program_model;
        Shader* _program_skybox;
        Shader* _program_screen; // post-processing

        Skybox _skybox;
        FrameBuffer _framebuffer;

        std::vector<PhysicEntity> _physicEntities;
        std::vector<Model> _models;

        PhysicEntity* _holdedEntity = nullptr;
        float _holdedDistance = 3.0f;
};
