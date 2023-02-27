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

struct Surrounding {
    u8 t, b;
    u8 te, tw, tn, ts;
    u8 me, mw, mn, ms;
    u8 be, bw, bn, bs;
    u8 tne, tnw, tse, tsw;
    u8 mne, mnw, mse, msw;
    u8 bne, bnw, bse, bsw;
};

void fillVerts(u32 *verts, u32 &count, u32 x, u32 y, u32 z, u8 c, const Surrounding &s);
