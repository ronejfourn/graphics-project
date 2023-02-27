#pragma once

#include "window/events.hpp"
#include "rendering/shader.hpp"
#include "rendering/shadowMap.hpp"
#include "world/world.hpp"
#include "scene/camera.hpp"
#include "scene/sky.hpp"

class Scene
{
public:
     Scene();
    ~Scene();
    void update(Events &e, f32 dt);
    void render();

private:
    struct {i32 w, h;} m_viewport;
    Camera m_camera;
    Shader m_blockShader;
    Shader m_depthShader;
    World m_world;
    Sky m_sky;
};
