#version 330 core
layout (location = 0) in uint aVert;

uniform vec2 xz;
uniform mat4 viewproj;

out vec3 vCol;

void main() {
    uint v = aVert;

    float x = float(v &  31u);
    v = v >> 5;
    float z = float(v &  31u);
    v = v >> 5;
    float y = float(v & 511u);
    v = v >> 9;
    uint t = (v & 255u);

    vCol = vec3(float(t & 1u), float((t >> 1) & 1u), float((t >> 2) & 1u));
    gl_Position = viewproj * vec4(
        x + xz.x * 16.0f,
        y,
        z + xz.y * 16.0f,
        1.0f);
}
