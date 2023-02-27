#include "scene/sky.hpp"
#include "glad/glad.h"
#include "world/chunk.hpp"

static constexpr f32 sunVertices[] = {
     1.0f, -0.1f, -0.1f,  1.0f, -0.1f,  0.1f,  1.0f,  0.1f,  0.1f,
     1.0f,  0.1f,  0.1f,  1.0f,  0.1f, -0.1f,  1.0f, -0.1f, -0.1f,
};

static constexpr f32 skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,
};

Sky::Sky(const Vec3 &pos) :
    m_sun {
        VertexArray(STATIC),
        ShadowMap(1, 4096, 4096),
        Shader("../shaders/sun.v.glsl", "../shaders/sun.f.glsl"),
    },
    m_skybox {
        VertexArray(STATIC),
        Shader("../shaders/skybox.v.glsl", "../shaders/skybox.f.glsl"),
        Cubemap(3, {
                "../resources/skybox/right.png",
                "../resources/skybox/left.png",
                "../resources/skybox/top.png",
                "../resources/skybox/bottom.png",
                "../resources/skybox/front.png",
                "../resources/skybox/back.png",
            }),
    }
{
    m_skybox.vao.bind();
    m_skybox.vao.setData(sizeof(skyboxVertices), (void *)skyboxVertices);

    VertexAttrib va = {0, 3, FLOAT};
    m_skybox.vao.setAttribs(1, &va);

    m_sun.proj = mat4Orthographic(240, 240, 256);
    m_sun.direction = Vec3(-1.0f,  0.0f,  0.0f);

    Vec3 dir = normalize(m_sun.direction);
    Vec3 src = pos;
    src.y = CHUNK_MAX_Y / 2.0f;
    m_sun.view = mat4LookAt(src, dir, Vec3(0, 1, 0));
    m_sun.direction = dir;

    m_skybox.shader.bind();
    m_skybox.shader.uniform("skybox", m_skybox.cubemap.getTextureUnit());

    m_sun.vao.bind();
    m_sun.vao.setData(sizeof(sunVertices), (void *)sunVertices);
    m_sun.vao.setAttribs(1, &va);
}

void Sky::update(const Camera &camera, f32 deltaTime)
{
    f32 deltaAngle = 0.00125f * deltaTime;

    static f32 accum = 0;
    float q = lerp(0.1f, 4.0f, sinf(DEG2RAD(m_sun.angle)) * 0.5f + 0.5f);
    if (accum > q) {
        const Mat4 rot  = mat4RotationZ(DEG2RAD(accum));
        m_sun.direction = Vec3(rot * Vec4(m_sun.direction));
        accum = 0;
    }
    m_sun.angle += deltaAngle;
    m_skybox.angle += deltaAngle;
    accum += deltaAngle;

    Vec3 dir = m_sun.direction;
    Vec4 src = Vec4(camera.getPosition());
    src.y = CHUNK_MAX_Y / 2.0f;

    Mat4 sunViewProj = m_sun.proj * m_sun.view;
    src = sunViewProj * src;
    src = src / src.w * 0.5f + Vec4(0.5f);
    src.x = roundf(src.x * m_sun.shadowMap.getWidth ()) / m_sun.shadowMap.getWidth ();
    src.y = roundf(src.y * m_sun.shadowMap.getHeight()) / m_sun.shadowMap.getHeight();
    src = src * 2.0f - Vec4(1.0f);

    Mat4 invSunViewProj = mat4Inverse(sunViewProj);
    src = invSunViewProj * Vec4(src);
    m_sun.view = mat4LookAt(Vec3(src), dir, Vec3(0, 1, 0));

    f32 diff;
    if (m_sun.angle < 45.0f)       diff = lerp(0.2f, 0.4f, (m_sun.angle -   0.0f) / ( 45.0f -   0.0f));
    else if (m_sun.angle <  75.0f) diff = lerp(0.4f, 0.6f, (m_sun.angle -  45.0f) / ( 75.0f -  45.0f));
    else if (m_sun.angle <  90.0f) diff = lerp(0.6f, 1.0f, (m_sun.angle -  75.0f) / ( 90.0f -  75.0f));
    else if (m_sun.angle < 105.0f) diff = lerp(1.0f, 0.6f, (m_sun.angle -  90.0f) / (105.0f -  90.0f));
    else if (m_sun.angle < 135.0f) diff = lerp(0.6f, 0.4f, (m_sun.angle - 105.0f) / (135.0f - 105.0f));
    else if (m_sun.angle < 180.0f) diff = lerp(0.4f, 0.2f, (m_sun.angle - 135.0f) / (180.0f - 135.0f));
    else if (m_sun.angle < 225.0f) diff = lerp(0.2f, 0.0f, (m_sun.angle - 180.0f) / (225.0f - 180.0f));
    else if (m_sun.angle < 315.0f) diff = 0;
    else if (m_sun.angle < 360.0f) diff = lerp(0.0f, 0.2f, (m_sun.angle - 315.0f) / (360.0f - 315.0f));
    else m_sun.angle = 0, diff = 0.2f;
    m_sun.diffuse = Vec3(diff);
    m_sun.ambient = Vec3(1.1f, 1.0f, 0.7f) * max(diff, 0.2f);
}

void Sky::render(const Camera &camera)
{
    Mat4 view = camera.getViewMatrix();
    view[0][3] = 0;
    view[1][3] = 0;
    view[2][3] = 0;
    Mat4 proj = camera.getProjectionMatrix();
    Mat4 viewproj = proj * view;
    Mat4 modelviewproj = viewproj * mat4RotationZ(DEG2RAD(m_sun.angle));
    Vec3 sunPos = -m_sun.direction;

    glDepthRange(0.9999f, 1.0f);

    m_sun.shader.bind();
    m_sun.shader.uniform("modelviewproj", modelviewproj);
    m_sun.shader.uniform("sunPos", sunPos);
    m_sun.vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    modelviewproj = viewproj * mat4RotationY(DEG2RAD(m_skybox.angle));
    m_skybox.vao.bind();
    m_skybox.shader.bind();
    m_skybox.shader.uniform("modelviewproj", modelviewproj);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthRange(0.0f, 1.0f);
}
