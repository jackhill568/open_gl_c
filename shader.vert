#version 330 core
in vec2 vPos;
uniform float time;
void main()
{
    vec2 pos = vPos; 
    //pos.x += sin(time) * 0.25;
    //pos.y -=tan(time) * 0.25;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0); 
}
