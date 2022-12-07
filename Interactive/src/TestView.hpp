#pragma once

#include <iostream>

#include "View.hpp"
#include "shader.h"
#include "BoidManager.hpp"
#include "Camera.hpp"

// #include <glm/glm.hpp>
// #include <glm/gtc/random.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

class TestView: public View {
    public:
        TestView(Context& ctx): View(ctx) {

            int width, height;
            glfwGetWindowSize(ctx.window, &width, &height);

            camera = new OrbitCamera(
                glm::vec3(0.0f), 0.0f, 0.0f, 10.0f,
                60.0f, (float)width / (float)height, 0.01f, 1000.0f
            );

            boid_shader = new Shader("./assets/boid.vs", "./assets/boid.fs");

            // boids
            boid_count = 10000;
            boidManager = new BoidManager(boid_count, glm::vec3(-40), glm::vec3(40));
            positions = new glm::vec4[boid_count];

            glGenBuffers(1, &ssbo_boid);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_boid);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 2 * boid_count, NULL, GL_STREAM_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
            // glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
            // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_boid); // bind ssbo to index


            const float vertices[] = {
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
                0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
                0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
                0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
            };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);


            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        // ~AppView() {}

        void onHideView() {}
        void onShowView() {}

        void onUpdate(float time_since_start, float dt)
        {
            boidManager->update();


            glBindBuffer(GL_ARRAY_BUFFER, ssbo_boid); // bind

            float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            memcpy(ptr, boidManager->boids.data(), sizeof(Boid) * boid_count);
            // for (int i = 0; i < boid_count ; ++i) {
            //     int index = i * 8; // 8 floats

            //     // ptr[index+0] = *glm::value_ptr(boidManager->boids[i].pos);
            //     ptr[index+0] = boidManager->boids[i].pos.x;
            //     ptr[index+1] = boidManager->boids[i].pos.y;
            //     ptr[index+2] = boidManager->boids[i].pos.z;
            //     // // ptr[index+3] // padding
            //     ptr[index+4] = boidManager->boids[i].dir.x;
            //     ptr[index+5] = boidManager->boids[i].dir.y;
            //     ptr[index+6] = boidManager->boids[i].dir.z;
            //     // ptr[index+7] // padding
            // }
            glUnmapBuffer(GL_ARRAY_BUFFER);

            // glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_boid); // bind
            // glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * boid_count, positions);
            // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

            boid_shader->setVec3("cameraPos", camera->getPosition());
        }

        void onDraw(float time_since_start, float dt)
        {
            // Rendering
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            boid_shader->use();
            boid_shader->setMat4("u_projectionMatrix", camera->getProjection());
            boid_shader->setMat4("u_viewMatrix", camera->getView());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_boid);

            glBindVertexArray(VAO);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6*6, boid_count);

            ctx.imguiNewFrame();
            ImGui::Begin("Debug");

            ImGui::Text("%.4f ms", dt);
            ImGui::Text("%.2f fps", 1.0f / dt);

            ImGui::Spacing();

            ImGui::SliderFloat(
                "view distance",
                &(boidManager->view_distance),
                0.0f, 10.0f,
                "%.3f"
            );

            float alignment_weight = 1.87f;
            float cohesion_weight = 1.34f;

            ImGui::SliderFloat(
                "view angle",
                &(boidManager->view_angle),
                0.0f, M_PI,
                "%.3f"
            );
            ImGui::SliderFloat(
                "Speed",
                &(boidManager->speed),
                0.0f, 0.3f,
                "%.3f"
            );
            ImGui::SliderFloat(
                "Separation weight",
                &(boidManager->separation_weight),
                0.0f, 10.0f,
                "%.3f"
            );
            ImGui::SliderFloat(
                "Alignment weight",
                &(boidManager->alignment_weight),
                0.0f, 10.0f,
                "%.3f"
            );
            ImGui::SliderFloat(
                "Cohesion weight",
                &(boidManager->cohesion_weight),
                0.0f, 10.0f,
                "%.3f"
            );


            ImGui::End();
            ctx.imguiRender();
        }

        void onKeyPress(int key)
        {
        }

        void onKeyRelease(int key)
        {
        }

        void onMouseMotion(int x, int y, int dx, int dy)
        {
        }

        void onMouseDrag(int x, int y, int dx, int dy)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureMouse)
                return;

            camera->setYaw( camera->getYaw() - (dx * 0.005f) );
            camera->setPitch( camera->getPitch() + (dy * 0.005f) );
        }

        void onMousePress(int x, int y, int button)
        {
        }

        void onMouseRelease(int x, int y, int button)
        {
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
        Shader* boid_shader;

        BoidManager* boidManager;
        glm::vec4* positions;

        GLuint ssbo_boid;
        unsigned int VAO, VBO;
};
