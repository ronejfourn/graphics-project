#include "scene/sky.hpp"
#include "glad/glad.h"
#include "world/chunk.hpp"

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
    m_sun { ShadowMap(1, 4096, 4096) },
    m_skybox {
        VertexArray(STATIC),
        Cubemap(3, {
                "../resources/skybox/right.png",
                "../resources/skybox/left.png",
                "../resources/skybox/top.png",
                "../resources/skybox/bottom.png",
                "../resources/skybox/front.png",
                "../resources/skybox/back.png",
            })
    },
    m_shader("../shaders/skybox.v.glsl", "../shaders/skybox.f.glsl")
{
    m_skybox.vao.bind();
    m_skybox.vao.setData(sizeof(skyboxVertices), (void *)skyboxVertices);

    VertexAttrib va = {0, 3, FLOAT};
    m_skybox.vao.setAttribs(1, &va);

    m_sun.proj = mat4Orthographic(256, 256, 256);
    m_sun.ambient   = Vec3( 0.8f,  0.8f,  0.7f);
    m_sun.diffuse   = Vec3( 0.9f,  0.9f,  0.8f);
    m_sun.direction = Vec3( 0.7f, -0.7f,  0.7f);

    Vec3 dir = normalize(m_sun.direction);
    Vec3 src = pos;
    src.y = CHUNK_MAX_Y / 2.0f;
    m_sun.view = mat4LookAt(src, dir, Vec3(0, 1, 0));
    m_sun.direction = dir;

    m_shader.bind();
    m_shader.uniform("skybox", m_skybox.cubemap.getTextureUnit());
}

void Sky::update(const Camera &camera, f32 deltaTime)
{
    /* f32 deltaAngle = 0.0012f * deltaTime; */

    /* static f32 accum = 0; */
    /* if (accum > 1) { */
    /*     const Mat4 rot  = mat4RotationZ(DEG2RAD(accum)); */
    /*     m_sun.direction = Vec3(rot * Vec4(m_sun.direction)); */
    /*     accum = 0; */
    /* } */
    /* accum += deltaAngle; */

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

    /* static f32 sunAngle = 0; */
    /* sunAngle += deltaAngle; */

    /* f32 diff; */
    /* if (sunAngle < 45.0f)       diff = lerp(0.2f, 0.4f, (sunAngle -   0.0f) / ( 45.0f -   0.0f)); */
    /* else if (sunAngle <  75.0f) diff = lerp(0.4f, 0.6f, (sunAngle -  45.0f) / ( 75.0f -  45.0f)); */
    /* else if (sunAngle <  90.0f) diff = lerp(0.6f, 1.0f, (sunAngle -  75.0f) / ( 90.0f -  75.0f)); */
    /* else if (sunAngle < 105.0f) diff = lerp(1.0f, 0.6f, (sunAngle -  90.0f) / (105.0f -  90.0f)); */
    /* else if (sunAngle < 135.0f) diff = lerp(0.6f, 0.4f, (sunAngle - 105.0f) / (135.0f - 105.0f)); */
    /* else if (sunAngle < 180.0f) diff = lerp(0.4f, 0.2f, (sunAngle - 135.0f) / (180.0f - 135.0f)); */
    /* else if (sunAngle < 225.0f) diff = lerp(0.2f, 0.0f, (sunAngle - 180.0f) / (225.0f - 180.0f)); */
    /* else if (sunAngle < 315.0f) diff = 0; */
    /* else if (sunAngle < 360.0f) diff = lerp(0.0f, 0.2f, (sunAngle - 315.0f) / (360.0f - 315.0f)); */
    /* else sunAngle = 0, diff = 0.2f; */
    /* m_sun.diffuse = Vec3(diff); */
    /* m_sun.ambient = max(Vec3(0.05f), m_sun.ambient * diff); */
}

void Sky::render(const Camera &camera)
{
    Mat4 view = camera.getViewMatrix();
    view[0][3] = 0;
    view[1][3] = 0;
    view[2][3] = 0;
    Mat4 proj = camera.getProjectionMatrix();
    Mat4 viewproj = proj * view;

    Vec3 sunPos = -m_sun.direction;
    m_shader.bind();
    m_shader.uniform("viewproj", viewproj);
    m_shader.uniform("sunPos", sunPos);

    m_skybox.vao.bind();
    glDepthRange(0.9999f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthRange(0.0f, 1.0f);
}
