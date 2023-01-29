#pragma once

#include "platform/events.hpp"
#include "rendering/camera.hpp"
#include "rendering/skybox.hpp"
#include "world/world.hpp"

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
