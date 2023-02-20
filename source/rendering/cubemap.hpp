#pragma once

#include "utility/common.hpp"

class Cubemap
{
public:
    struct Paths {
        const char* paths[6];
    };

    Cubemap(u32, Paths);
    ~Cubemap();

    inline u32 getTextureUnit() const { return m_unit; }
private:
    u32 m_texture;
    u32 m_unit;
};
