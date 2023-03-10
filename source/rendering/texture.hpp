#pragma once

#include "utility/common.hpp"
#include"textureArray.hpp"

class Texture2D
{
public:
    Texture2D(const char *path);
    void bind();
    void destroy();
private:
    u32 m_texture;
};
