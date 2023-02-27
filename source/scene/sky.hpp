#pragma once

#include "rendering/vertexArray.hpp"
#include "rendering/shadowMap.hpp"
#include "rendering/shader.hpp"
#include "rendering/cubemap.hpp"
#include "scene/camera.hpp"

struct Sun {
    VertexArray vao;
    ShadowMap shadowMap;
    Shader shader;
    Mat4 view      = Mat4();
    Mat4 proj      = Mat4();
    Vec3 direction = Vec3();
    Vec3 ambient   = Vec3();
    Vec3 diffuse   = Vec3();
    f32 angle      = 0.0f;
};

struct SkyBox {
    VertexArray vao;
    Shader shader;
    Cubemap cubemap;
    f32 angle = 0.0f;
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
};
