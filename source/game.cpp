#include "game.hpp"
#include "glad/glad.h"

Game &Game::instance()
{
    static Game s_instance;
    return s_instance;
}

Game::Game() :
    ////////////////////////////////////////////////////////////////////////////////
    // Texture [Risav]
    m_skybox(
        "../shaders/skybox.v.glsl", "../shaders/skybox.f.glsl",
        {
            "../resources/skybox/right.png",
            "../resources/skybox/left.png",
            "../resources/skybox/top.png",
            "../resources/skybox/bottom.png",
            "../resources/skybox/front.png",
            "../resources/skybox/back.png",
        }),
    ////////////////////////////////////////////////////////////////////////////////
    m_world("../shaders/basic.v.glsl", "../shaders/basic.f.glsl", 32)
{
    glClearColor(.18f, .18f, .18f, .18f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    m_world.generate(123456789);
}

Game::~Game() {
}

void Game::update(Events &events)
{
    u32 direction = 0;

    if (events.keyPressed(KEY_ESCAPE)) events.quit = true;

    if (events.keyHeld(KEY_W)) direction |= FORWARD;
    if (events.keyHeld(KEY_A)) direction |= LEFT;
    if (events.keyHeld(KEY_S)) direction |= BACKWARD;
    if (events.keyHeld(KEY_D)) direction |= RIGHT;
    m_camera.processKeyboard((CameraMovement)direction, 0.1f);

    if (events.keyHeld(KEY_H)) m_camera.processMouseMovement(-0.5,  0);
    if (events.keyHeld(KEY_J)) m_camera.processMouseMovement( 0, -0.5);
    if (events.keyHeld(KEY_K)) m_camera.processMouseMovement( 0,  0.5);
    if (events.keyHeld(KEY_L)) m_camera.processMouseMovement( 0.5,  0);

    if (events.window.resized)
    {
        glViewport(0, 0, events.window.w, events.window.h);
        m_camera.setAspectRatio((f32)events.window.w / (f32)events.window.h);
    }

    m_world.update(m_camera.getPosition());
}

void Game::render()
{
    glClear(GL_DEPTH_BUFFER_BIT);

    m_world.render(m_camera);

    // draw skybox
    m_skybox.render(m_camera);
}
