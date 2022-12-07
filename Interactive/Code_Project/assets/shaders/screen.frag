#version 330 core

out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D texture_color;
uniform sampler2D texture_depth;

uniform vec2 resolution;
uniform int post_processing_id;

float linearize_depth(float z, float near, float far)
{
    return (2.0 * near) / (far + near - z * (far - near));
}

vec3 chromaticAberation();

void main()
{
    vec3 color;

    switch (post_processing_id) {
        case 0:
            color = texture(texture_color, tex_coords).rgb;
            break;
        case 1:
            color = chromaticAberation();
            break;
        case 2:
            color = 1.0 - texture(texture_color, tex_coords).rgb;
            break;
        default:
            color = texture(texture_color, tex_coords).rgb;
            break;
    }

    float depth = texture(texture_depth, tex_coords).r;
    depth = linearize_depth(depth, 0.01, 100.0);

    // FragColor = vec4(vec3(depth), 1.0);
    FragColor = vec4(color, 1.0);
}

vec3 chromaticAberation()
{
    vec2 uv = (gl_FragCoord.xy / resolution.xy);
    uv = (uv * 2.0) - 1.0;

    vec2 delta = normalize(vec2(0.0, 0.0) - uv);

    float l = length(uv);
    delta *= 0.02 * l;

    float r = texture(texture_color, tex_coords - delta).r;
    float g = texture(texture_color, tex_coords).g;
    float b = texture(texture_color, tex_coords + delta).b;

    return vec3(r, g, b);

}