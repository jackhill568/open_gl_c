#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout(location = 0) in vec3 vpos;
layout(location = 3) in vec3 aNormal;
layout(location = 6) in vec2 aTexCoords;


layout(location = 5)out vec3 FragPos;
layout(location = 4)out vec3 Normal;
layout(location = 2) out vec2 TexCoords;
void main() {
    gl_Position = proj * view * model * vec4(vpos, 1.0);
    FragPos = vec3(model * vec4(vpos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords =vec2(aTexCoords);
}

