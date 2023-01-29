#pragma once

#include "utility/common.hpp"
#include "rendering/shader.hpp"
#include "rendering/vertexArray.hpp"

union Mat4;
class Camera;

class Skybox
{
public:
    struct Paths {
        const char* paths[6];
    };

    Skybox(const char *vshader, const char *fshader, Paths);
    void render(Camera &camera);
    ~Skybox();
private:
    u32 m_texture;
    Shader m_shader;
    VertexArray m_vao;
};
