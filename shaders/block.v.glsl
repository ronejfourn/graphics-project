#version 330 core
layout (location = 0) in uint aVert;

uniform vec2 xz;
uniform mat4 viewproj;

#define ONES(n) ((1u << n) - 1u)

out vec3 uvw;

void main() {
    uint v = aVert;

    float x = float(v & ONES(5));
    v = v >> 5;
    float z = float(v & ONES(5));
    v = v >> 5;
    float y = float(v & ONES(9));
    v = v >> 9;
    uint uv = v & ONES(2);
    v = v >> 2;
    uint w  = v & ONES(8);

    uvw = vec3(float((uv >> 1u) & 1u), float(uv & 1u), float(w));
    gl_Position = viewproj * vec4(
        x + xz.x * 16.0f,
        y,
        z + xz.y * 16.0f,
        1.0f);
}
