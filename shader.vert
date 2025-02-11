#version 460 core
layout(location = 0) in vec2 vpos;
layout(location = 2) in vec2 vtex;

layout(location = 1) out vec2 texCoord;

void main() {
    texCoord = vtex;
    gl_Position = vec4(vpos, 0.0, 1.0);
}
