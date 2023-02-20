#pragma once

#include "rendering/vertexArray.hpp"
#include "rendering/shadowMap.hpp"
#include "rendering/shader.hpp"
#include "rendering/cubemap.hpp"
#include "scene/camera.hpp"

struct Sun {
    ShadowMap shadowMap;
    Mat4 view, proj;
    Vec3 direction;
    Vec3 ambient;
    Vec3 diffuse;
};

struct SkyBox {
    VertexArray vao;
    Cubemap cubemap;
};

class Sky {
public:
    Sky(const Vec3 &pos);

    void update(const Camera &cam, f32 dt);
    void render(const Camera &cam);
    inline const Sun &getSun() { return m_sun; }
    inline const SkyBox &getSkybox() { return m_skybox; }

private:
    Sun m_sun;
    SkyBox m_skybox;
    Shader m_shader;
};
