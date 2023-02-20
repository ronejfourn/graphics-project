#pragma once

#include "utility/common.hpp"

class ShadowMap {
public:
    ShadowMap(
        u32 unit   = 0,
        i32 width  = 1024,
        i32 height = 1024
    );
    ~ShadowMap();

    inline u32 getTextureUnit() const { return m_textureUnit; }
    inline u32 getWidth () const { return m_width ; }
    inline u32 getHeight() const { return m_height; }
    void prepWrite() const;
private:
    i32 m_width, m_height;
    u32 m_framebuffer;
    u32 m_texture, m_textureUnit;
};
