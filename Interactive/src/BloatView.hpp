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

            program = new Shader("./assets/object.vs", "./assets/object.fs");
            // _model.loadModel("assets/table/table.gltf");
            _model.loadModel("assets/maxwell_the_cat/scene.gltf");

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

            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            program->use();

            program->setVec3("view_pos", camera->getPosition());
            program->setMat4("view_matrix", camera->getView());
            program->setMat4("proj_matrix", camera->getProjection());
            _model.draw(*program);

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
        Shader* program;

        Model _model;

        unsigned int VAO, VBO;
};
