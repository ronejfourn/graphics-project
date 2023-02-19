#version 330 core
out vec4 frg;

in float aof;
in vec3  nrm;
in vec3  uvw;
in vec4 lPos;
in float flogz;

uniform sampler2DArray texArray;
uniform sampler2D shadowMap;

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

const float ZFAR = 1000000.0;
const float FCOEF = 2.0 / log2(ZFAR + 1.0);
const float HALF_FCOEF = 0.5 * FCOEF;

const float fogDensity = 0.00005f;
const vec4  fogColor   = vec4(0.6f, 0.8f, 1.0f, 1.0f);

uniform DirectionalLight sun;

float getShadow(float d) {
    vec3 lproj = lPos.xyz / lPos.w;
    lproj = lproj * 0.5f + 0.5f;
    if (lproj.z > 1 || d <= 0) return 1.0f;

    float shadow = 0;
    vec2 texel = 1.0f / textureSize(shadowMap, 0);
    float bias = max(0.004f * (1 - d), 0.001f);
    for (int y = -3; y <= 3; y ++) {
        for (int x = -3; x <= 3; x ++) {
            float depth = texture(shadowMap, lproj.xy + vec2(x * texel.x, y * texel.y)).r;
            shadow += lproj.z - bias > depth ? 0.0f : 1.0f;;
        }
    }

    return shadow / 49.0f;
}

void main() {
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * sun.ambient;

    float dotp = dot(nrm, sun.direction);
    float diff = max(-dotp, 0.0f);
    vec3 diffuse = diff * sun.diffuse;
    float shadow = getShadow(diff);

    vec4 tex = texture(texArray, uvw);
    vec3 lit = (ambient * aof + shadow * diffuse);
    vec3 alb = tex.rgb;
    vec3 clr = alb * lit;

    float z = gl_FragCoord.z / gl_FragCoord.w;
    float f = clamp(exp(-fogDensity * z * z), 0.2, 1);

    clr = pow(clr, vec3(1.0f / 2.0f));
    frg = mix(fogColor, vec4(clr, tex.a), f);

    gl_FragDepth = log2(flogz) * HALF_FCOEF;
}
