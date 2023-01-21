#include <iostream>
#include "png.hpp"
#include "texture.hpp"

Texture2D::Texture2D()
{
    float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture2D::setImageFromFile(const char *path)
{
    u32 widthImg, heightImg;
    u8 *bytes = loadPNGFromFile(path, &widthImg, &heightImg);
    if (!bytes) {
        std::cerr << getPNGError() << std::endl;
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(bytes);
    return true;
}

void Texture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
}
