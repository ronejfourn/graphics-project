#pragma once

#include "utility/common.hpp"

class TextureArray
{
private:
    u32 tileX;
    u32 tileY;
    u32 id;
    u32 unit;
public:
    TextureArray(u32,const char*,int,int);
    inline u32 getTextureUnit() const { return unit; }
    void bind();
};
