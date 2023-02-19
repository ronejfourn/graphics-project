#version 330 core
layout (location = 0) in uint aVert;

uniform vec2 xz;
uniform mat4 sunViewProj;

#define ONES(n) ((1u << n) - 1u)

void main() {
    uint v = aVert;

    float x = float(v & ONES(4));
    v = v >> 4;
    float z = float(v & ONES(4));
    v = v >> 4;
    float y = float(v & ONES(8));

    vec3 pos = vec3(x + xz.x, y, z + xz.y);
    gl_Position = sunViewProj * vec4(pos, 1.0f);
    if (gl_Position.z < -1) gl_Position.z = -1;
}
