#pragma once

#include "utility/common.hpp"

class TextureArray
{
private:
    u32 tileX;
    u32 tileY;
    u32 id;
public:
    TextureArray(const char*,int,int);
    void bind();
};
