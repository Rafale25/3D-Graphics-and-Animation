#version 430 core

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;

in vec3 in_position;
in vec3 in_normal;

out vec3 v_fragPos;
out vec3 v_normal;

struct Boid {
    vec4 pos;
    vec4 vel;
}; // don't forget padding for std430

layout(std430, binding=0) buffer positionBuffer
{
    Boid boids[];
};


mat3 rotateY(float value) {
    return mat3(
        cos(value), 0.0, sin(value),
        0.0, 1.0, 0.0,
        -sin(value), 0.0, cos(value)
    );
}

mat3 rotateZ(float value) {
    return mat3(
        cos(value), -sin(value), 0.0,
        sin(value), cos(value), 0.0,
        0.0, 0.0, 1.0
    );
}

#define PI 3.141592
const float size = 0.2;

void main() {
    vec3 pos = boids[gl_InstanceID].pos.xyz;
    vec3 dir = boids[gl_InstanceID].vel.xyz;

    float yaw = atan(dir.z, dir.x);
    float pitch = abs(atan(sqrt(dir.x*dir.x + dir.z*dir.z), dir.y)) - PI/2;

    mat3 rotY = rotateY(yaw);
    mat3 rotZ = rotateZ(pitch);
    mat3 model = rotY * rotZ;

    vec3 worldPos = (model * in_position*size) + pos;
    vec4 position = u_projectionMatrix * u_viewMatrix * vec4(worldPos, 1.0);

    v_fragPos = worldPos;
    v_normal = mat3(transpose(inverse(model))) * in_normal;
    // v_normal = model * in_normal; // don't know if it can be done just like that

    gl_Position = position;
}