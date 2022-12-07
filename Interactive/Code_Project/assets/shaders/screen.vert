#version 330 core

layout (location = 0) in vec2 in_vertex;
layout (location = 1) in vec2 in_uv;

out vec2 tex_coords;

void main()
{
    tex_coords = in_uv;
    gl_Position = vec4(in_vertex.x, in_vertex.y, 0.0, 1.0);
}