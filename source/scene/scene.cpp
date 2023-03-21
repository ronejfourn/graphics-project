#include "scene.hpp"
#include "glad/glad.h"
#include "math/matrix.hpp"
#include <cstdio>

static const Vec3 DEF_CAMERA_POS(-100, 140, 50);
constexpr u32 RENDER_DISTANCE = 16;

static bool cullFace = true;

Scene::Scene() :
    m_camera(DEF_CAMERA_POS, 90, 1, Vec3(0, 1, 0), -89),
    m_blockShader("../shaders/block.v.glsl", "../shaders/block.f.glsl"),
    m_depthShader("../shaders/depth.v.glsl", "../shaders/depth.f.glsl"),
    m_world(2 * RENDER_DISTANCE),
    m_sky(DEF_CAMERA_POS)
{
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glPolygonOffset(4, 4);
    glClearColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const Sun &sun = m_sky.getSun();
    const TextureArray &ta = m_world.getTextureArray();
    const SkyBox &sb = m_sky.getSkybox();
    m_blockShader.bind();
    m_blockShader.uniform("texArray", ta.getTextureUnit());
    m_blockShader.uniform("skybox", sb.cubemap.getTextureUnit());
    m_blockShader.uniform("shadowMap", sun.shadowMap.getTextureUnit());

    m_blockShader.uniform("settings.doEnvMap"  , true);
    m_blockShader.uniform("settings.doLighting", true);
    m_blockShader.uniform("settings.doShadow"  , true);
    m_blockShader.uniform("settings.doAO"      , true);
    m_blockShader.uniform("settings.doFog"     , true);

    f32 far = magnitude(Vec3(0, 0, 0) - Vec3((RENDER_DISTANCE + 1) * 15.0f, 0, 0));
    m_camera.setPlanes(0.0001f, far);
    m_camera.setAspectRatio((f32)1280 / (f32)720);

    m_world.generate(123456789, DEF_CAMERA_POS);
}

Scene::~Scene() {
}

void Scene::update(const Events &events, f32 deltaTime)
{
    u32 direction = 0;
    if (events.keyHeld(KEY_W) || events.keyHeld(KEY_UP   )) direction |= FORWARD;
    if (events.keyHeld(KEY_A) || events.keyHeld(KEY_LEFT )) direction |= LEFT;
    if (events.keyHeld(KEY_S) || events.keyHeld(KEY_DOWN )) direction |= BACKWARD;
    if (events.keyHeld(KEY_D) || events.keyHeld(KEY_RIGHT)) direction |= RIGHT;
    m_camera.processKeyboard((CameraMovement)direction, deltaTime);

    float r = 0.025f * deltaTime;
    if (events.keyHeld(KEY_H)) m_camera.processMouseMovement(-r,  0);
    if (events.keyHeld(KEY_J)) m_camera.processMouseMovement( 0, -r);
    if (events.keyHeld(KEY_K)) m_camera.processMouseMovement( 0,  r);
    if (events.keyHeld(KEY_L)) m_camera.processMouseMovement( r,  0);

    m_blockShader.bind();
    if (events.keyPressed(KEY_1)){
        static bool b = true;
        m_blockShader.uniform("settings.doEnvMap"  , b = !b);
        printf("settings.doEnvMap = %s\n"  , b ? "true" : "false");
    }

    if (events.keyPressed(KEY_2)){
        static bool b = true;
        m_blockShader.uniform("settings.doLighting", b = !b);
        printf("settings.doLighting = %s\n", b ? "true" : "false");
    }

    if (events.keyPressed(KEY_3)){
        static bool b = true;
        m_blockShader.uniform("settings.doShadow"  , b = !b);
        printf("settings.doShadow = %s\n"  , b ? "true" : "false");
    }

    if (events.keyPressed(KEY_4)){
        static bool b = true;
        m_blockShader.uniform("settings.doAO"      , b = !b);
        printf("settings.doAO = %s\n"      , b ? "true" : "false");
    }

    if (events.keyPressed(KEY_5)){
        static bool b = true;
        m_blockShader.uniform("settings.doFog"     , b = !b);
        printf("settings.doFog = %s\n"     , b ? "true" : "false");
    }

    if (events.keyPressed(KEY_6)){
        cullFace = !cullFace;
        printf("cullFace = %s\n"   , cullFace ? "true" : "false");
    }

    f32 s = 1;
    if (events.keyHeld(KEY_F)) s = 64;

    if (events.keyPressed(KEY_T)) {
        static bool wf = true;
        glPolygonMode(GL_FRONT_AND_BACK, wf ? GL_LINE : GL_FILL);
        wf = !wf;
    }

    m_viewport.w = events.window.w;
    m_viewport.h = events.window.h;
    if (events.window.resized)
        m_camera.setAspectRatio((f32)events.window.w / (f32)events.window.h);

    m_world.update(m_camera.getPosition());

    static bool rev = true;
    if (events.keyPressed(KEY_R))
        rev = !rev;

    m_sky.update(m_camera, deltaTime * s * rev);
}

void Scene::render()
{
    const Sun &sun = m_sky.getSun();
    Mat4 sunViewProj = sun.proj * sun.view;
    Mat4 camViewProj = m_camera.getProjectionMatrix() * m_camera.getViewMatrix();

    m_depthShader.bind();
    m_depthShader.uniform("sunViewProj", sunViewProj);

    // world shadow pass
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    sun.shadowMap.prepWrite();
    m_world.depthPass(m_depthShader, sunViewProj);
    glDisable(GL_POLYGON_OFFSET_FILL);

    if (cullFace) glEnable(GL_CULL_FACE);

    // world render pass
    glViewport(0, 0, m_viewport.w, m_viewport.h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_blockShader.bind();
    m_blockShader.uniform("camPos", m_camera.getPosition());
    m_blockShader.uniform("camViewProj", camViewProj);
    m_blockShader.uniform("sunViewProj", sunViewProj);

    m_blockShader.uniform("sun.ambient", sun.ambient);
    m_blockShader.uniform("sun.diffuse", sun.diffuse);
    m_blockShader.uniform("sun.direction", sun.direction);

    m_world.renderPass(m_blockShader, camViewProj);

    // draw skybox
    m_sky.render(m_camera);
}
