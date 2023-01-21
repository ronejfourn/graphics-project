#pragma once

#include <glad/glad.h>

class Texture2D
{
private:
    GLuint m_texture;
public:
    Texture2D();
    ~Texture2D() { glDeleteTextures(1, &m_texture); }

    bool setImageFromFile(const char *path);
    void bind();
};
