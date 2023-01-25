#pragma once

#include "events.hpp"
#include "math.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "camera.hpp"

class Game
{
public:
    static Game &instance();
    void update(Events &e);
    void render();
private:
    Game();
    Game(Game &) = delete;
    ~Game();
    Shader m_shader;
    Camera m_camera;
    Texture2D m_textures[2];
    u32 m_vao, m_vbo, m_ebo;
};
