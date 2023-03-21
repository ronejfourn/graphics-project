#version 330 core
layout (location = 0) in uint aVert;

uniform vec2 xz;
uniform vec3 camPos;
uniform mat4 camViewProj;
uniform mat4 sunViewProj;

#define ONES(n) ((1u << n) - 1u)

out vec3 normal;
out vec3 texCoord;
out vec4 lsPos;
out vec3 viewDir;
out float aoFactor;
out float projZ;

float aoArr[4] = float[4](0.25f, 0.5f, 0.75f, 1.0f);

vec3 norms[3] = vec3[3](
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f)
);

const float ZFAR = 1000000.0;
const float FCOEF = 4.0 / log2(ZFAR + 1.0);

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

    aoFactor = aoArr[ao];
    normal = norms[n] * (1.0f - float(s) * 2.0f);
    texCoord = vec3(float((uv >> 1u) & 1u), float(uv & 1u), float(w));

    vec3 pos = vec3(x + xz.x, y, z + xz.y);
    gl_Position = camViewProj * vec4(pos, 1.0f);
    projZ = gl_Position.z;
    gl_Position.z = (log2(max(1e-6, 1.0 + gl_Position.w)) * FCOEF - 1.0) * gl_Position.w;

    lsPos = sunViewProj * vec4(pos, 1.0f);
    viewDir = normalize(pos - camPos);
}
