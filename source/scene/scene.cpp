#include "scene.hpp"
#include "glad/glad.h"
#include "math/matrix.hpp"

static struct {
    Mat4 view, proj;
    Vec3 direction;
    Vec3 ambient;
    Vec3 diffuse;
} sun;

static i32 W = DEFAULT_WINDOW_WIDTH;
static i32 H = DEFAULT_WINDOW_HEIGHT;
static const Vec3 DEF_CAMERA_POS(0, 140, 0);
constexpr u32 RENDER_DISTANCE = 16;

Scene &Scene::instance()
{
    static Scene s_instance;
    return s_instance;
}

Scene::Scene() :
    m_camera(DEF_CAMERA_POS, 90, 1, Vec3(0, 1, 0), DEG2RAD(-89)),
    m_blockShader("../shaders/block.v.glsl", "../shaders/block.f.glsl"),
    m_depthShader("../shaders/depth.v.glsl", "../shaders/depth.f.glsl"),
    ////////////////////////////////////////////////////////////////////////////////
    // Texture [Risav]
    m_skybox(
        {
            "../resources/skybox/right.png",
            "../resources/skybox/left.png",
            "../resources/skybox/top.png",
            "../resources/skybox/bottom.png",
            "../resources/skybox/front.png",
            "../resources/skybox/back.png",
        }),
    ////////////////////////////////////////////////////////////////////////////////
    m_shadowMap(1, 8192, 8192),
    m_world(2 * RENDER_DISTANCE)
{
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glClearColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_world.generate(123456789, DEF_CAMERA_POS);

    m_blockShader.bind();
    m_blockShader.uniform("texArray", 0);
    m_blockShader.uniform("shadowMap", m_shadowMap.getTextureUnit());

    f32 far = magnitude(Vec3(0, 0, 0) - Vec3((RENDER_DISTANCE + 1) * 15.0f, 0, 0));
    m_camera.setPlanes(0.0001f, far);

    sun.proj = mat4Orthographic(128, 128, 128);
    sun.ambient   = Vec3( 1.0f);
    sun.diffuse   = Vec3( 1.0f);
    sun.direction = Vec3(-1.0f, 0.0f, 0.0f);

    Vec3 dir = normalize(sun.direction);
    Vec3 src = m_camera.getPosition();
    src.y = 128.0f;
    sun.view = mat4LookAt(src, dir, Vec3(0, 1, 0));
    sun.direction = dir;
}

Scene::~Scene() {
}

void Scene::update(Events &events)
{
    if (events.keyPressed(KEY_ESCAPE)) events.quit = true;

    u32 direction = 0;
    if (events.keyHeld(KEY_W)) direction |= FORWARD;
    if (events.keyHeld(KEY_A)) direction |= LEFT;
    if (events.keyHeld(KEY_S)) direction |= BACKWARD;
    if (events.keyHeld(KEY_D)) direction |= RIGHT;
    m_camera.processKeyboard((CameraMovement)direction, 0.1f);

    if (events.keyHeld(KEY_H)) m_camera.processMouseMovement(-0.5,  0);
    if (events.keyHeld(KEY_J)) m_camera.processMouseMovement( 0, -0.5);
    if (events.keyHeld(KEY_K)) m_camera.processMouseMovement( 0,  0.5);
    if (events.keyHeld(KEY_L)) m_camera.processMouseMovement( 0.5,  0);

    if (events.window.resized) {
        W = events.window.w, H = events.window.h;
        m_camera.setAspectRatio((f32)events.window.w / (f32)events.window.h);
    }

    m_world.update(m_camera.getPosition());

    static f32 accum = 0;
    if (accum > 1) {
        accum = 0;
        const Mat4 rot = mat4RotationZ(DEG2RAD(1));
        sun.direction  = Vec3(rot * Vec4(sun.direction));
    }
    accum += 0.2f;

    Vec3 dir = sun.direction;
    Vec4 src = Vec4(m_camera.getPosition());
    src.y = 128.0f;

    Mat4 sunViewProj = sun.proj * sun.view;
    src = sunViewProj * src;
    src = src / src.w * 0.5f + Vec4(0.5f);

    src.x = roundf(src.x * m_shadowMap.getWidth ()) / m_shadowMap.getWidth ();
    src.y = roundf(src.y * m_shadowMap.getHeight()) / m_shadowMap.getHeight();
    src = src * 2.0f - Vec4(1.0f);

    Mat4 invSunViewProj = mat4Inverse(sunViewProj);
    src = invSunViewProj * Vec4(src);

    sun.view = mat4LookAt(Vec3(src), dir, Vec3(0, 1, 0));
}

void Scene::render()
{
    Mat4 sunViewProj = sun.proj * sun.view;
    Mat4 camViewProj = m_camera.getProjectionMatrix() * m_camera.getViewMatrix();

    m_depthShader.bind();
    m_depthShader.uniform("sunViewProj", sunViewProj);

    // world shadow pass
    m_shadowMap.prepWrite();
    m_world.depthPass(m_depthShader);

    // world render pass
    glViewport(0, 0, W, H);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_blockShader.bind();
    m_blockShader.uniform("camViewProj", camViewProj);
    m_blockShader.uniform("sunViewProj", sunViewProj);

    m_blockShader.uniform("sun.ambient", sun.ambient);
    m_blockShader.uniform("sun.diffuse", sun.diffuse);
    m_blockShader.uniform("sun.direction", sun.direction);

    m_world.renderPass(m_blockShader);

    // draw skybox
    m_skybox.render(m_camera);
}
