#version 460 core
uniform sampler2D currentstate;
uniform vec2 resolution;
layout(location = 0) out vec4 fragment;
layout(location = 1) in vec2 texCoord;

void main() {
    float currentCell = texture(currentstate, texCoord).r;
    
    vec2 offsets[8] = vec2[](
        vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0, 0.0),                   vec2(1.0, 0.0),
        vec2(-1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 1.0)
    );
    
    float liveNeighbors = 0.0;
    for (int i = 0; i < 8; i++) {
        vec2 neighborUV = texCoord + offsets[i] / resolution;
        liveNeighbors += texture(currentstate, neighborUV).r;
    }
    
    // Conway's Game of Life rules
    float nextState;
    if (currentCell > 0.5) {
        // Cell is alive
        nextState = (liveNeighbors >= 2.0 && liveNeighbors <= 3.0) ? 1.0 : 0.0;
    } else {
        // Cell is dead
        nextState = (liveNeighbors == 3.0) ? 1.0 : 0.0;
    }
    
    fragment = vec4(nextState, nextState, nextState, 1.0);
}
