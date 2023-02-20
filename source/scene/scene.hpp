#pragma once

#include "platform/events.hpp"
#include "rendering/shader.hpp"
#include "rendering/shadowMap.hpp"
#include "world/world.hpp"
#include "scene/camera.hpp"
#include "scene/sky.hpp"

class Scene
{
public:
    static Scene &instance();
    void update(Events &e, f32 dt);
    void render();
private:
    Scene();
    Scene(Scene &) = delete;
    ~Scene();
    Camera m_camera;
    Shader m_blockShader;
    Shader m_depthShader;
    World m_world;
    Sky m_sky;
};
