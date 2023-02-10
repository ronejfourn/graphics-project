#pragma once

#include "utility/common.hpp"
#define BLOCK_TEXTURE_FILE       "../resources/texture.png"
#define BLOCK_TILES_PER_ROW      4
#define BLOCK_TILES_PER_COLUMN   2

enum BlockType {
    AIR = 0,
    GRASS,
    DIRT,
    SAND,
    WATER,
    OAKTREETRUNK,
    OAKLEAF,

    _BLOCK_TYPE_MAX_
};

void fillVerts(u32 *verts, u32 &count, u16 x, u16 y, u16 z, u8 type,
        u8 sb, u8 nb, u8 eb, u8 wb, u8 tb, u8 bb);
