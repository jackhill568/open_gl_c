#version 460 core
layout(location=2) in vec3 outColour;
layout(location=0) out vec4 fragment;
layout(location=4) in vec3 Normal;
layout(location=5) in vec3 FragPos;

uniform vec3 lightColour;
uniform vec3 lightPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); 

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * outColour;    float ambientStrength = 0.1;

    vec3 ambient = ambientStrength * lightColour;

    vec3 result = (ambient + diffuse) * outColour;
    fragment = vec4(result, 1.0);
}
