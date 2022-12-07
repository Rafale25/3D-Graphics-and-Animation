#version 330 core

layout (location = 0) in vec3 in_vertex;

out VS_OUT {
    vec3 tex_coords;
} vs_out;

uniform mat4 proj_matrix;
uniform mat4 view_matrix;

void main()
{
    vs_out.tex_coords = in_vertex;
    vec4 pos = proj_matrix * view_matrix * vec4(in_vertex, 1.0);
    gl_Position = pos.xyww;
}