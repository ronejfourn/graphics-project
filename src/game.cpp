#include "game.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include <stdio.h>

Game& Game::instance()
{
    static Game s_instance;
    return s_instance;
}

Game::Game() :
    m_shader("../shaders/basic.v.glsl", "../shaders/basic.f.glsl"),

    ////////////////////////////////////////////////////////////////////////////////

    // Texture [Risav]
    m_textures{
       {"../resources/check.png"},
       {"../resources/check.png"}
    }

    ////////////////////////////////////////////////////////////////////////////////
{
    f32 verts[] = {
        // coordinates          /        colors           /     texturelocation
         0.5f,  0.5f,  0.5f, /*****/ 0.0f, 0.0f, 0.0f, /*****/ 1.0f, 0.0f, // 0
         0.5f,  0.5f, -0.5f, /*****/ 0.0f, 0.0f, 1.0f, /*****/ 0.0f, 0.0f, // 1
         0.5f, -0.5f,  0.5f, /*****/ 0.0f, 1.0f, 0.0f, /*****/ 1.0f, 1.0f, // 2
         0.5f, -0.5f, -0.5f, /*****/ 0.0f, 1.0f, 1.0f, /*****/ 0.0f, 1.0f, // 3

        -0.5f,  0.5f,  0.5f, /*****/ 1.0f, 0.0f, 0.0f, /*****/ 0.0f, 0.0f, // 4
        -0.5f,  0.5f, -0.5f, /*****/ 1.0f, 0.0f, 1.0f, /*****/ 1.0f, 0.0f, // 5
        -0.5f, -0.5f,  0.5f, /*****/ 1.0f, 1.0f, 0.0f, /*****/ 0.0f, 1.0f, // 6
        -0.5f, -0.5f, -0.5f, /*****/ 1.0f, 1.0f, 1.0f, /*****/ 1.0f, 1.0f, // 7

         0.5f,  0.5f,  0.5f, /*****/ 0.0f, 0.0f, 0.0f, /*****/ 1.0f, 0.0f, // 8
         0.5f,  0.5f, -0.5f, /*****/ 0.0f, 0.0f, 1.0f, /*****/ 0.0f, 0.0f, // 9
        -0.5f,  0.5f,  0.5f, /*****/ 1.0f, 0.0f, 0.0f, /*****/ 1.0f, 1.0f, // 10
        -0.5f,  0.5f, -0.5f, /*****/ 1.0f, 0.0f, 1.0f, /*****/ 0.0f, 1.0f, // 11

         0.5f, -0.5f,  0.5f, /*****/ 0.0f, 1.0f, 0.0f, /*****/ 0.0f, 0.0f, // 12
         0.5f, -0.5f, -0.5f, /*****/ 0.0f, 1.0f, 1.0f, /*****/ 1.0f, 0.0f, // 13
        -0.5f, -0.5f,  0.5f, /*****/ 1.0f, 1.0f, 0.0f, /*****/ 0.0f, 1.0f, // 14
        -0.5f, -0.5f, -0.5f, /*****/ 1.0f, 1.0f, 1.0f, /*****/ 1.0f, 1.0f, // 15
    };

    GLuint indxs[] = {
        3, 1, 5,   3, 5, 7,
        6, 4, 0,   6, 0, 2,

        7, 5, 4,   7, 4, 6,
        2, 0, 1,   2, 1, 3,

        10, 11,  9,  10,  9,  8,
        15, 14, 12,  15, 12, 13,
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indxs), indxs, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(3 * sizeof(f32)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glClearColor(.18f, .18f, .18f, .18f);
    glBindVertexArray(m_vao);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

}

Game::~Game() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

void Game::update(Events &events) {
    u32 direction = 0;
    if (events.keyHeld(KEY_W))
        direction |= FORWARD;
    if (events.keyHeld(KEY_A))
        direction |= LEFT;
    if (events.keyHeld(KEY_S))
        direction |= BACKWARD;
    if (events.keyHeld(KEY_D))
        direction |= RIGHT;
    m_camera.processKeyboard((CameraMovement)direction, 0.02f);

    if (events.keyHeld(KEY_H))
        m_camera.processMouseMovement(-0.5,  0);
    if (events.keyHeld(KEY_J))
        m_camera.processMouseMovement( 0, -0.5);
    if (events.keyHeld(KEY_K))
        m_camera.processMouseMovement( 0,  0.5);
    if (events.keyHeld(KEY_L))
        m_camera.processMouseMovement( 0.5,  0);

    if (events.window.resized) {
        glViewport(0, 0, events.window.w, events.window.h);
        m_camera.setAspectRatio((f32)events.window.w / (f32)events.window.h);
    }

    GLuint umx  = m_shader.uniform("mx");
    GLuint umy  = m_shader.uniform("my");
    glUniform1i(umx, events.cursor.x);
    glUniform1i(umy, events.cursor.y);
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float a = 0;
    a += 0.01f;
    float c = cosf(a);
    float s = sinf(a);

    Mat4 rotX = mat4RotationX(c, s);
    Mat4 rotY = mat4RotationY(c, s);
    Mat4 rotZ = mat4RotationZ(c, s);
    Mat4 trns = mat4Translation(0, 0, 1.5);

    Mat4 proj = m_camera.getProjectionMatrix();
    Mat4 view = m_camera.getViewMatrix();
    Mat4 model = trns * rotX * rotY;

    m_shader.bind();
    Mat4 mvp = proj * view * model;
    GLuint uMVP = m_shader.uniform("mvp");
    glUniformMatrix4fv(uMVP, 1, GL_TRUE, &mvp[0][0]);

    glBindVertexArray(m_vao);
    m_textures[0].bind();
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void *)0);
    m_textures[1].bind();
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void *)(24 * 4));
}
