#version 330 core


uniform vec3 triangleColour;
out vec4 fragment;
void main()
{
    fragment = vec4(triangleColour.r * 0.5, triangleColour.g, triangleColour.b + 0.4, 1.0);
}
