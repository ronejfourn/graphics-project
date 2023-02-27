#version 330 core
out vec4 frg;

in vec3 normal;
in vec3 texCoord;
in vec3 viewDir;
in vec4 lsPos;
in float aoFactor;
in float flogz;

uniform sampler2DArray texArray;
uniform sampler2D shadowMap;
uniform samplerCube skybox;

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

const float ZFAR = 1000000.0;
const float FCOEF = 2.0 / log2(ZFAR + 1.0);
const float HALF_FCOEF = 0.5 * FCOEF;

uniform DirectionalLight sun;

float getShadow(float d) {
    vec3 lproj = lsPos.xyz / lsPos.w;
    lproj = lproj * 0.5f + 0.5f;
    if (lproj.z > 1 || d <= 0) return 1.0f;

    float depth = texture(shadowMap, lproj.xy).r;
    return (lproj.z - 0.00125f > depth ? 0.0f : 1.0f);
}

vec3 applyFog(vec3 rgb)
{
    const float fogDensity = 0.00003f;
    float sunAmount = max(-dot(viewDir, sun.direction), 0.0) * mix(1.0f, 0.0f, min(abs(sun.direction.y) / 0.6f, 1.0f));
    vec3  fogColor  = mix(vec3(0.5f, 0.6f, 0.8f),
                          vec3(1.1f, 1.0f, 0.7f),
                          sunAmount * sunAmount);
    float z = gl_FragCoord.z / gl_FragCoord.w;
    float fogAmount = 1 - clamp(exp(-fogDensity * z * z), 0.2, 1);
    return mix(rgb, fogColor, fogAmount);
}

vec3 calcLight()
{
    float ambientStrength = 0.4f;
    vec3 ambient = ambientStrength * sun.ambient;

    float dotp = dot(normal, sun.direction);
    float diff = max(-dotp, 0.0f);
    vec3 diffuse = diff * sun.diffuse;

    float shininess = 16.0f + 112.0f * float(texCoord.z == 6);
    float specularStrength = 0.5f;
    vec3 reflectDir = reflect(-sun.direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * sun.diffuse;

    float shadow = getShadow(diff);

    return (ambient + shadow * (diffuse + specular)) * aoFactor;
}

void main() {
    vec4 tex = texture(texArray, texCoord);
    vec3 alb = tex.rgb;
    vec3 lit = calcLight();

    vec3 clr = alb;
    vec3 R = reflect(viewDir, normal);
    clr = mix(clr, texture(skybox, R).rgb, (0.5f * float(texCoord.z == 6)));
    clr = clr * lit;
    clr = applyFog(clr);
    clr = pow(clr, vec3(1.0f / 1.5f));
    frg = vec4(clr, tex.a);

    gl_FragDepth = log2(flogz) * HALF_FCOEF;
}
