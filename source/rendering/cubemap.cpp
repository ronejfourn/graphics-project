#include "glad/glad.h"
#include "utility/png.hpp"
#include "rendering/cubemap.hpp"

Cubemap::Cubemap(u32 u, Paths p)
{
    m_unit = u;
    u32 width, height;
    glGenTextures(1, &m_texture);
    glActiveTexture(GL_TEXTURE0 + u);
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
    glActiveTexture(GL_TEXTURE0);
}


Cubemap::~Cubemap()
{
    glDeleteTextures(1, &m_texture);
}
