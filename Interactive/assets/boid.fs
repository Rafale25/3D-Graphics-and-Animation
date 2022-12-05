#version 430 core

in vec3 v_normal;
in vec3 v_fragPos;

out vec4 fragColor;

uniform vec3 sunDir = vec3(0.1, 1.0, 0.3);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 cameraPos;

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(sunDir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * vec3(1.0, 0.0, 0.0);
    fragColor = vec4(result, 1.0);
}