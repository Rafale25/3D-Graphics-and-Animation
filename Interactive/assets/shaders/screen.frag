#version 330 core

out vec4 FragColor;

in vec2 tex_coords;

uniform vec2 resolution;
uniform sampler2D screen_texture;
uniform int post_processing_id = -1;

vec3 chromaticAberation();

void main()
{
    vec3 color;

    switch (post_processing_id) {
        case 0:
            color = chromaticAberation();
            break;
        default:
            color = texture(screen_texture, tex_coords).rgb;
            break;
    }

    FragColor = vec4(color, 1.0);
}

vec3 chromaticAberation()
{
    vec2 uv = (gl_FragCoord.xy / resolution.xy);
    uv = (uv * 2.0) - 1.0;

    vec2 delta = normalize(vec2(0.0, 0.0) - uv);

    float l = length(uv);
    delta *= 0.02 * l;

    float r = texture(screen_texture, tex_coords - delta).r;
    float g = texture(screen_texture, tex_coords).g;
    float b = texture(screen_texture, tex_coords + delta).b;

    return vec3(r, g, b);
}