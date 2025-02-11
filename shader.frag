#version 460 core
uniform sampler2D currentstate;
uniform vec2 resolution;
uniform vec2 mousePos;
layout(location = 0) out vec4 fragment;
layout(location = 1) in vec2 texCoord;

void main() {
    vec2 MousePos = vec2(mousePos.x, mousePos.y +60);
    vec2 normalizedMousePos = MousePos / resolution;
    float tolerance = 0.0012;
    if (abs(texCoord.x - normalizedMousePos.x) < tolerance && abs(texCoord.y - (1.- normalizedMousePos.y)) < tolerance) {
        fragment = vec4(1.0); 
        return;
    }


    float currentCell = texture(currentstate, texCoord).r;
    
    vec2 offsets[8] = vec2[](
        vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0, 0.0),                   vec2(1.0, 0.0),
        vec2(-1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 1.0)
    );
    
    float liveNeighbors = 0.0;
    for (int i = 0; i < 8; i++) {
        vec2 neighborUV = texCoord + offsets[i] / resolution;
        liveNeighbors += (texture(currentstate, neighborUV).r > 0) ? 1. : 0.;
    }
    
    // Conway's Game of Life rules
    float nextState;
    if (currentCell > 0.5) {
        // Cell is alive
        nextState = (liveNeighbors <= 3.0 && liveNeighbors >=2) ? currentCell-0.001 : 0.0;
        fragment = vec4(nextState, nextState, nextState, 1.0);
    } else {
        // Cell is dead
        nextState = (liveNeighbors == 3.0) ? 1.0 : 0.0;
        fragment = vec4(nextState, 0.0, 0.0, 1.0);
    }
    
}
