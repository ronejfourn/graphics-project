#version 330 core
layout (location = 0) in uint aVert;

uniform vec2 xz;
uniform mat4 camViewProj;
uniform mat4 sunViewProj;

#define ONES(n) ((1u << n) - 1u)

out float aof;
out vec3  nrm;
out vec3  uvw;
out vec4  lPos;
out float flogz;

float aoArr[4] = float[4](0.25f, 0.5f, 0.75f, 1.0f);

vec3 norms[3] = vec3[3](
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f)
);

const float ZFAR = 1000000.0;
const float FCOEF = 2.0 / log2(ZFAR + 1.0);
const float HALF_FCOEF = 0.5 * FCOEF;

void main() {
    uint v = aVert;

    float x = float(v & ONES(4));
    v = v >> 4;
    float z = float(v & ONES(4));
    v = v >> 4;
    float y = float(v & ONES(8));
    v = v >> 8;
    uint n  = v & ONES(2);
    v = v >> 2;
    uint s  = v & ONES(1);
    v = v >> 1;
    uint uv = v & ONES(2);
    v = v >> 2;
    uint ao = v & ONES(2);
    v = v >> 2;
    uint w  = v & ONES(8);

    aof = aoArr[ao];
    nrm = norms[n] * (1.0f - float(s) * 2.0f);
    uvw = vec3(float((uv >> 1u) & 1u), float(uv & 1u), float(w));

    vec3 pos = vec3(x + xz.x, y, z + xz.y);
    gl_Position = camViewProj * vec4(pos, 1.0f);
    lPos = sunViewProj * vec4(pos, 1.0f);

    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * FCOEF - 1.0;
    flogz = 1.0 + gl_Position.w;
}
