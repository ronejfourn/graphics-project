#include "scene.hpp"
#include "glad/glad.h"
#include "math/matrix.hpp"

static i32 W = DEFAULT_WINDOW_WIDTH;
static i32 H = DEFAULT_WINDOW_HEIGHT;
static const Vec3 DEF_CAMERA_POS(0, 140, 0);
constexpr u32 RENDER_DISTANCE = 32;

Scene &Scene::instance()
{
    static Scene s_instance;
    return s_instance;
}

Scene::Scene() :
    m_camera(DEF_CAMERA_POS, 90, 1, Vec3(0, 1, 0), DEG2RAD(-89)),
    m_blockShader("../shaders/block.v.glsl", "../shaders/block.f.glsl"),
    m_depthShader("../shaders/depth.v.glsl", "../shaders/depth.f.glsl"),
    m_world(2 * RENDER_DISTANCE),
    m_sky(DEF_CAMERA_POS)
{
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glClearColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const Sun &sun = m_sky.getSun();
    const TextureArray &ta = m_world.getTextureArray();
    const SkyBox &sb = m_sky.getSkybox();
    m_blockShader.bind();
    m_blockShader.uniform("texArray", ta.getTextureUnit());
    m_blockShader.uniform("skybox", sb.cubemap.getTextureUnit());
    m_blockShader.uniform("shadowMap", sun.shadowMap.getTextureUnit());

    f32 far = magnitude(Vec3(0, 0, 0) - Vec3((RENDER_DISTANCE + 1) * 15.0f, 0, 0));
    m_camera.setPlanes(0.0001f, far);

    m_world.generate(123456789, DEF_CAMERA_POS);
}

Scene::~Scene() {
}

void Scene::update(Events &events, f32 deltaTime)
{
    if (events.keyPressed(KEY_ESCAPE)) events.quit = true;

    u32 direction = 0;
    if (events.keyHeld(KEY_W)) direction |= FORWARD;
    if (events.keyHeld(KEY_A)) direction |= LEFT;
    if (events.keyHeld(KEY_S)) direction |= BACKWARD;
    if (events.keyHeld(KEY_D)) direction |= RIGHT;
    m_camera.processKeyboard((CameraMovement)direction, deltaTime);

    float r = 0.025f * deltaTime;
    if (events.keyHeld(KEY_H)) m_camera.processMouseMovement(-r,  0);
    if (events.keyHeld(KEY_J)) m_camera.processMouseMovement( 0, -r);
    if (events.keyHeld(KEY_K)) m_camera.processMouseMovement( 0,  r);
    if (events.keyHeld(KEY_L)) m_camera.processMouseMovement( r,  0);

    if (events.window.resized) {
        W = events.window.w, H = events.window.h;
        m_camera.setAspectRatio((f32)events.window.w / (f32)events.window.h);
    }

    m_world.update(m_camera.getPosition());
    m_sky.update(m_camera, deltaTime);
}

void Scene::render()
{
    const Sun &sun = m_sky.getSun();
    Mat4 sunViewProj = sun.proj * sun.view;
    Mat4 camViewProj = m_camera.getProjectionMatrix() * m_camera.getViewMatrix();

    m_depthShader.bind();
    m_depthShader.uniform("sunViewProj", sunViewProj);

    // world shadow pass
    sun.shadowMap.prepWrite();
    m_world.depthPass(m_depthShader);

    // world render pass
    glViewport(0, 0, W, H);
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
