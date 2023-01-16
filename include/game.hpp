#pragma once

#include "events.hpp"
#include "camera.hpp"
#include "skybox.hpp"
#include "world.hpp"

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
    Camera m_camera;
    Skybox m_skybox;
    World m_world;
};
