#version 330 core


uniform vec3 triangleColour;
out vec4 fragment;
void main()
{
    fragment = vec4(triangleColour, 1.0);
}
