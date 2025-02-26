#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec3 colour;

layout(location = 2)out vec3 outColour;

void main() {
    gl_Position = proj * view * model * vec4(vpos, 1.0);
    outColour = colour;
}
