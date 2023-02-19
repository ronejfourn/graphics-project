#pragma once

#include "platform/events.hpp"
#include "rendering/skybox.hpp"
#include "rendering/shader.hpp"
#include "rendering/shadowMap.hpp"
#include "world/world.hpp"
#include "scene/camera.hpp"

class Scene
{
public:
    static Scene &instance();
    void update(Events &e);
    void render();
private:
    Scene();
    Scene(Scene &) = delete;
    ~Scene();
    Camera m_camera;
    Shader m_blockShader;
    Shader m_depthShader;
    Skybox m_skybox;
    ShadowMap m_shadowMap;
    World m_world;
};
