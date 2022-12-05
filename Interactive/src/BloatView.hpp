#pragma once

#include <iostream>
#include <vector>

#include "View.hpp"
#include "shader.h"
#include "Camera.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// #include <glm/glm.hpp>
// #include <glm/gtc/random.hpp>
// #include <glm/gtc/matrix_transform.hpp>

#include "Model.hpp"

class BloatView: public View {
    public:
        BloatView(Context& ctx): View(ctx) {

            int width, height;
            glfwGetWindowSize(ctx.window, &width, &height);

            camera = new OrbitCamera(
                glm::vec3(0.0f),
                0.0f, 0.0f, 5.0f,
                60.0f, (float)width / (float)height, 0.01f, 1000.0f
            );

            // program_cubemap = new Shader("./assets/cubemap.vert", "./assets/cubemap.frag");

            program_model = new Shader("./assets/object.vs", "./assets/object.fs");
            Model model_maxwell = Model();
            model_maxwell.loadModel("assets/maxwell_the_cat/scene.gltf");
            model_maxwell.transform.scale *= 0.1f;
            model_maxwell.transform.position.z = 2.0f;

            Model model_helmet = Model();
            model_helmet.loadModel("../../../Downloads/glTF-Sample-Models-master/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");
            // model_helmet.transform.scale *= 0.8f;
            // model_helmet.transform.position.z = 2.0f;

            _models.push_back(model_maxwell);
            _models.push_back(model_helmet);

            // _model.loadModel("assets/maxwell_the_cat/scene.gltf");
            // _model.loadModel("../../../Downloads/glTF-Sample-Models-master/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");
            // _model.loadModel("../../../Downloads/glTF-Sample-Models-master/2.0/Sponza/glTF/Sponza.gltf");
        }
        // ~BloatView() {}

        void onHideView() {}
        void onShowView() {}

        void onUpdate(float time_since_start, float dt)
        {
        }

        void onDraw(float time_since_start, float dt)
        {
            // Rendering
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            program_model->use();

            program_model->setMat4("view_matrix", camera->getView());
            program_model->setMat4("proj_matrix", camera->getProjection());

            program_model->setVec3("view_pos", camera->getPosition());
            program_model->setVec3("light_positions[0]", camera->getPosition());
            program_model->setVec3("light_colors[0]", glm::vec3(1.0, 1.0, 1.0));

            for (Model& model : _models) {
                model.draw(*program_model);
            }

            ctx.imguiNewFrame();
            ImGui::Begin("Debug");

            ImGui::Text("%.4f ms", dt);
            ImGui::Text("%.2f fps", 1.0f / dt);

            ImGui::Spacing();

            ImGui::End();
            ctx.imguiRender();
        }

        void onMouseDrag(int x, int y, int dx, int dy)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureMouse)
                return;

            camera->setYaw( camera->getYaw() - (dx * 0.005f) );
            camera->setPitch( camera->getPitch() + (dy * 0.005f) );
        }

        void onMouseScroll(int scroll_x, int scroll_y)
        {
            camera->setDistance( camera->getDistance() - (scroll_y * 0.2f) );
        }

        void onResize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }

    private:
        int boid_count;
        OrbitCamera* camera;

        Shader* program_model;
        Shader* program_cubemap;

        std::vector<Model> _models;
        // Model _model;

        unsigned int VAO, VBO;
};
