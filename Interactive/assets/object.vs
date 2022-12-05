#version 410 core

layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normals;
layout (location = 2) in vec3 in_tangents;
layout (location = 3) in vec2 in_uv;

out VS_OUT {
    vec3 frag_pos;
    vec2 tex_coords;
    vec3 tangent_light_pos;
    vec3 tangent_view_pos;
    vec3 tangent_frag_pos;
} vs_out;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

uniform vec3 view_pos;
const vec3 light_pos = vec3(20.0, 20.0, 20.0);

void main(void)
{
    vec4 pos = model_matrix * vec4(in_vertex, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(model_matrix)));
    vec3 T = normalize(normalMatrix * in_tangents);
    vec3 N = normalize(normalMatrix * in_normals);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.tangent_light_pos = TBN * light_pos;
    vs_out.tangent_view_pos  = TBN * view_pos;
    vs_out.tangent_frag_pos  = TBN * pos.xyz;

    vs_out.tex_coords = in_uv;

    gl_Position = proj_matrix * view_matrix * pos;
}

// #version 410 core

// layout (location = 0) in vec3 in_vertex;
// layout (location = 1) in vec3 in_normals;
// layout (location = 2) in vec2 in_uv;

// out VS_OUT
// {
//     vec3 vertex;
//     vec3 normals;
//     vec2 uv;
// } vs_out;


// uniform mat4 model_matrix;
// uniform mat4 view_matrix;
// uniform mat4 proj_matrix;

// void main(void)
// {
//     gl_Position = proj_matrix * view_matrix * model_matrix * vec4(in_vertex, 1.0);

//     // mat3 normalMatrix = transpose(inverse(mat3(model_matrix)));

//     vs_out.vertex = in_vertex;
//     vs_out.normals = in_normals;
//     vs_out.uv = in_uv;
// }