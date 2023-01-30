#include "glad/glad.h"
#include "math/matrix.hpp"
#include "utility/png.hpp"
#include "rendering/skybox.hpp"
#include "rendering/camera.hpp"

static constexpr f32 skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,
};


Skybox::Skybox(const char *vshader, const char *fshader, Paths p) :
    m_vao(STATIC),
    m_shader(vshader, fshader)
{
    u32 width, height;
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    for (u32 i = 0; i < 6; i++) {
        u8 *data = loadPNGFromFile(p.paths[i], &width, &height);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
                         width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            free(data);
        } else {
            die("while creating texture (%s) :\n%s",
                p.paths[i], getPNGError());
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_vao.bind();
    m_vao.setData(sizeof(skyboxVertices), (void *)skyboxVertices);

    VertexAttrib va = {0, 3, FLOAT};
    m_vao.setAttribs(1, &va);
}

void Skybox::render(Camera &camera)
{
    Mat4 view = camera.getViewMatrix();
    view[0][3] = 0;
    view[1][3] = 0;
    view[2][3] = 0;
    Mat4 proj = camera.getProjectionMatrix();
    Mat4 viewproj = proj * view;

    glDepthRange(0.9999f, 1.0f);
    m_shader.bind();
    m_shader.uniform("viewproj", viewproj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthRange(0.0f, 1.0f);
}

Skybox::~Skybox()
{
    glDeleteTextures(1, &m_texture);
}
