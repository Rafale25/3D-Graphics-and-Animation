#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

using glm::vec3;

struct Boid {
    vec3 pos;
    float padding1;
    vec3 dir;
    uint hash;
    // float padding2;
};

class BoidManager {
    public:
        BoidManager(uint count, vec3 pmin, vec3 pmax):
            count(count),
            pmin(pmin),
            pmax(pmax)
        {
            boids.resize(count);
            randomize();

            counter.resize(count);
            bucket.resize(count);
        }

        ~BoidManager() {
        }

        void randomize() { // randomize positions and direction of all boids
            for (auto &boid : boids) {
                boid.pos = glm::linearRand(pmin, pmax);
                boid.dir = glm::ballRand(1.0f);
            }
        }

        glm::ivec3 cellCoord(glm::vec3& p, float size) {
            size = std::max(size, 0.5f);
            return glm::ivec3( glm::floor(p/size) );
        }

        float hashCoords(glm::ivec3& cell, int count) {
            const uint MAX_SIZE = uint(pow(count, 1.0/3.0)); // cube root of the amount of boids
            uint i = (cell.z * MAX_SIZE*MAX_SIZE) +
            (cell.y * MAX_SIZE) +
            cell.x;

            // const h = y*675 + x
            return i % count;
        }

        void computeAccelerationStructure() {
            std::fill(counter.begin(), counter.end(), 0); // reset counter at 0

            // counting + update hash
            for (auto& boid: boids) {
                glm::ivec3 cell = cellCoord(boid.pos, view_distance);
                boid.hash = hashCoords(cell, count);
                counter[boid.hash] += 1;
            }

            // prefix sum
            for (int i = 1 ; i < count ; ++i) {
                counter[i] += counter[i - 1];
            }

            // fill in
            for (auto& boid: boids) {
                counter[boid.hash] -= 1;
                bucket[ counter[boid.hash] ] = &boid;
            }
        }

        void update() {
            computeAccelerationStructure();

            for (auto& me : boids) {
                int nb_neighbours = 0;
                vec3 average_position(0.0f);
                vec3 average_direction(0.0f);
                vec3 total_force(0.0f);

                vec3 new_pos = me.pos + (me.dir * speed);
                vec3 new_dir = me.dir;

                for (int z = -1 ; z <= 1 ; ++z) {
                for (int y = -1 ; y <= 1 ; ++y) {
                for (int x = -1 ; x <= 1 ; ++x) {
                    glm::ivec3 cell = cellCoord(me.pos, view_distance);

                    cell += glm::ivec3(x, y, z);

                    int cell_hash = hashCoords(cell, count);
                    int start = counter[cell_hash];
                    int end = counter[cell_hash + 1];

                    for (int i = start ; i < count && i < end; ++i) {
                        Boid& other = *(bucket[i]);
                        if (&me == &other) continue;

                        vec3 delta = new_pos - other.pos;
                        float square_distance = dot(delta, delta);

                        if (square_distance < view_distance*view_distance) {

                            float r = dot((other.pos - new_pos)/glm::length(other.pos - new_pos), new_dir/glm::length(new_dir));
                            if (glm::length(acos(r)) <= view_angle) {
                                nb_neighbours += 1;
                                average_direction += other.dir;
                                average_position += other.pos;
                                total_force += delta / square_distance;
                            }
                        }

                    }

                }
                }
                }

                new_dir += total_force * separation_weight;

                if (nb_neighbours != 0) {
                    // alignment
                    average_direction /= nb_neighbours;
                    new_dir += average_direction * alignment_weight;

                    // cohesion
                    average_position /= nb_neighbours;
                    new_dir += -(new_pos - average_position) * cohesion_weight;
                }

                vec3 np = new_pos + (new_dir * speed);
                float map_size = 80.0f;
                float mp2 = map_size * 0.5f;
                if (np.x < -mp2 || np.x > mp2 || np.y < -mp2 || np.y > mp2 || np.z < -mp2 || np.z > mp2) {
                    vec3 to_center = normalize(vec3(0.0f) - new_pos);
                    new_dir += to_center * 0.5f;
                }

                me.pos = new_pos;
                me.dir = normalize(new_dir);
            }
        }

    // private:
    public:
        float view_distance = 2.0f;
        float view_angle = 2.08f;
        float speed = 0.06f;

        float separation_weight = 0.59f;
        float alignment_weight = 1.87f;
        float cohesion_weight = 1.34f;
        // float separationDistance;
        // float aligmentDistance;
        // float cohesionDistance;

        // Cuboid in what the boids needs to stay in
        vec3 pmin;
        vec3 pmax;

        int count;
        std::vector<Boid> boids;

        // Spatial hashing
        std::vector<uint> counter;
        std::vector<Boid*> bucket;
};
