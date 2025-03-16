#version 460 core
layout(location=2) in vec3 outColour;
layout(location=0) out vec4 fragment;


void main() {
  fragment = vec4(outColour, 1.0f);
}
