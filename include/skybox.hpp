#pragma once

#include "common.hpp"
#include "shader.hpp"

union Mat4;

class Skybox
{
private:
    u32 m_texture, m_vao, m_vbo;
    u32 m_uVP;
    Shader m_shader;
public:
    struct Paths {
        const char* paths[6];
    };

    Skybox(const char *vshader, const char *fshader, Paths);
    void render(Mat4 &viewproj);
    ~Skybox();
};
