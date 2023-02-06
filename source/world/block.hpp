#pragma once

#include "utility/common.hpp"

enum BlockType {
    AIR = 0,
    GRASS,
    WATER,

    _BLOCK_TYPE_MAX_
};

void fillVerts(u32 *verts, u32 &count, u16 x, u16 y, u16 z, u8 type,
        u8 sb, u8 nb, u8 eb, u8 wb, u8 tb, u8 bb);
