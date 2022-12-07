#version 420 core

out vec4 FragColor;

#define PI 3.14159265359

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coords;
} fs_in;

// material parameters
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic_roughness;
uniform sampler2D texture_ambiant_occlusion; // G:roughness; B:metalness
uniform sampler2D texture_emission;

uniform samplerCube skybox;

uniform bool enableSunlight;
uniform bool enablePointLights;

// sun (pure white)
uniform vec3 sunDirection;
uniform float sunIntensity;

// point lights (colored)
uniform int nb_lights;
uniform vec3 light_positions[32];
uniform vec3 light_colors[32];
uniform float light_intensities[32];

uniform vec3 view_pos;

// const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(texture_normal, fs_in.tex_coords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.frag_pos);
    vec3 Q2  = dFdy(fs_in.frag_pos);
    vec2 st1 = dFdx(fs_in.tex_coords);
    vec2 st2 = dFdy(fs_in.tex_coords);

    vec3 N   = normalize(fs_in.normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

vec3 reflectance(vec3 radiance, vec3 N, vec3 V, vec3 L, vec3 H, vec3 F0, vec3 albedo, float roughness, float metallic)
{
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    return Lo;
}

void main()
{
    vec4 base_color = texture(texture_diffuse, fs_in.tex_coords).rgba;

    if (base_color.a < 0.5) // hack to get transparency working without sorting in z order
        discard;

    vec3 albedo     = pow(base_color.rgb, vec3(2.2));
    // vec3 albedo     = base_color.rgb;
    vec2 metallic_roughness = texture(texture_metallic_roughness, fs_in.tex_coords).gb;
    float roughness = metallic_roughness.x;
    float metallic  = metallic_roughness.y;
    float ao        = 1.0;//texture(texture_ambiant_occlusion, fs_in.tex_coords).r;
    vec3 emission = texture(texture_emission, fs_in.tex_coords).rgb;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(view_pos - fs_in.frag_pos);
    vec3 R = reflect(-V, N); // for skybox reflection

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    // point light
    if (enablePointLights) {
        for(int i = 0; i < nb_lights; ++i) {
            vec3 L = normalize(light_positions[i] - fs_in.frag_pos);
            vec3 H = normalize(V + L);
            float distance = length(light_positions[i] - fs_in.frag_pos);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = light_colors[i] * light_intensities[i] * attenuation;

            Lo += reflectance(radiance, N, V, L, H, F0, albedo, roughness, metallic);
        }
    }

    // sun (directional)
    if (enableSunlight)
    {
        vec3 L = -normalize(sunDirection);
        vec3 H = normalize(V + L);
        vec3 sunColor = vec3(1.0, 1.0, 1.0);
        vec3 radiance = sunColor * sunIntensity;

        Lo += reflectance(radiance, N, V, L, H, F0, albedo, roughness, metallic);
    }

    // reflectance equation
    // vec3 Lo = vec3(0.0);
    // for(int i = 0; i < nb_light; ++i)
    // {
    //     // calculate per-light radiance

    //     // point light
    //     // vec3 L = normalize(light_positions[i] - fs_in.frag_pos);
    //     // vec3 H = normalize(V + L);
    //     // float distance = length(light_positions[i] - fs_in.frag_pos);
    //     // float attenuation = 1.0 / (distance * distance);
    //     // vec3 radiance = light_colors[i] * light_intensities[i] * attenuation;

    //     // directional
    //     vec3 L = -normalize(vec3(.2, -.8, -.5));
    //     vec3 H = normalize(V + L);
    //     float sunIntensity = 14.0;
    //     vec3 radiance = light_colors[i] * sunIntensity;

    //     // Cook-Torrance BRDF
    //     float NDF = DistributionGGX(N, H, roughness);
    //     float G   = GeometrySmith(N, V, L, roughness);
    //     vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    //     vec3 numerator    = NDF * G * F;
    //     float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    //     vec3 specular = numerator / denominator;

    //     // kS is equal to Fresnel
    //     vec3 kS = F;
    //     // for energy conservation, the diffuse and specular light can't
    //     // be above 1.0 (unless the surface emits light); to preserve this
    //     // relationship the diffuse component (kD) should equal 1.0 - kS.
    //     vec3 kD = vec3(1.0) - kS;
    //     // multiply kD by the inverse metalness such that only non-metals
    //     // have diffuse lighting, or a linear blend if partly metal (pure metals
    //     // have no diffuse light).
    //     kD *= 1.0 - metallic;

    //     // scale light by NdotL
    //     float NdotL = max(dot(N, L), 0.0);

    //     // add to outgoing radiance Lo
    //     Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    // }

    // skybox reflection
    vec3 skyboxColor = texture(skybox, R).rgb;

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    float skyFactor = (1.0-roughness) * metallic;
    ambient += skyboxColor * albedo * skyFactor; // bullshit reflection maths (also add to the overall intensity which is pretty low by default)

    emission *= step(0.075, length(emission)); // remove artefacts from emission

    vec3 color = ambient + Lo + emission; //emission is not in the original code


    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    // color = pow(color, vec3(1.0/2.6));

    FragColor = vec4(color, base_color.a);
    // FragColor = vec4(skyboxColor, base_color.a);
    // FragColor = vec4(emission, base_color.a);
    // FragColor = vec4(base_color);
    // FragColor = vec4(vec3(ao), base_color.a);
    // FragColor = vec4(vec3(skyFactor), base_color.a);
    // FragColor = vec4(vec3(metallic), base_color.a);
    // FragColor = vec4(N, 1.0);
}

// #version 410 core

// out vec4 FragColor;

// in VS_OUT {
//     vec3 frag_pos;
//     vec2 tex_coords;
//     vec3 tangent_light_pos;
//     vec3 tangent_view_pos;
//     vec3 tangent_frag_pos;
// } fs_in;

// uniform sampler2D texture_diffuse;
// uniform sampler2D texture_normal;
// uniform sampler2D texture_metallic_roughness;

// // const vec3 sun = -normalize(vec3(.2, -.8, -.5));
// // const vec3 sun_color = vec3(1.0, .5, .3);
// const float ambient_strength = .15;

// void main()
// {
//     // obtain normal from normal map in range [0,1]
//     vec3 normal = texture(texture_normal, fs_in.tex_coords).rgb;
//     // transform normal vector to range [-1,1]
//     normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

//     // get diffuse color
//     vec4 color = texture(texture_diffuse, fs_in.tex_coords).rgba;
//     // ambient
//     vec3 ambient = 0.1 * color.rgb;
//     // diffuse
//     vec3 light_dir = normalize(fs_in.tangent_light_pos - fs_in.tangent_frag_pos);
//     float diff = max(dot(light_dir, normal), 0.0);
//     vec3 diffuse = diff * color.rgb;
//     // specular
//     vec3 view_dir = normalize(fs_in.tangent_view_pos - fs_in.tangent_frag_pos);
//     vec3 reflect_dir = reflect(-light_dir, normal);
//     vec3 halfway_dir = normalize(light_dir + view_dir);
//     float spec = pow(max(dot(normal, halfway_dir), 0.0), 32.0);// * (texture(texture_metallic_roughness, fs_in.tex_coords).b)); // use g and b

//     vec3 specular = vec3(0.2) * spec;
//     // FragColor = vec4(ambient + diffuse + specular, color.a);
//     // FragColor = vec4(normal, color.a);
//     // FragColor = vec4(light_dir, color.a);
//     FragColor = color;
// }

// #version 410 core

// out vec4 FragColor;

// in VS_OUT
// {
//     vec3 vertex;
//     vec3 normals;
//     vec2 uv;
// } fs_in;

// uniform sampler2D texture_diffuse;
// uniform sampler2D texture_normal;

// const vec3 sun = -normalize(vec3(.2, -.8, -.5));
// // const vec3 sun_color = vec3(1.0, .5, .3);
// const vec3 sun_color = vec3(1.0, 1.0, 1.0);
// const float ambient_strength = .15;

// void main(void)
// {
//     // vec3 normal = texture(texture_normal, fs_in.uv).rgb;
//     vec3 normal = fs_in.normals;

//     //color = vec4(fs_in.normals, 1.0);
//     vec3 ambient = ambient_strength * sun_color;

//     // float theta = max(dot(sun, normal), 0);
//     float theta = max(dot(sun, fs_in.normals), 0.0);

//     // vec3 color = vec3(fs_in.uv, 0.0);
//     // vec3 color = ambient + texture(texture_diffuse, fs_in.uv).rgb;
//     vec3 color = texture(texture_diffuse, fs_in.uv).rgb;
//     color *= theta;
//     // vec3 color = texture(texture_diffuse, fs_in.uv).rgb;
//     // FragColor = vec4(color, 1.0);

//     // vec4 c = vec4((fs_in.normals+1.0)/2.0, 1.0);
//     // vec3 color = vec3(fs_in.uv, 0.0);
//     // vec3 color = vec3(theta, 0.0, 0.0);

//     FragColor = vec4(color, 1.0);
// }
