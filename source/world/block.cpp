#include "block.hpp"
#include "chunk.hpp"
#define ONES(n) ((1 << n) - 1)

static_assert(_BLOCK_TYPE_MAX_ < 256, "too many block types");

static struct {
    u8 t, s, b;
} blockIndex[_BLOCK_TYPE_MAX_] = {
    {       }, // AIR
    {0, 1, 2}, // GRASS
    {2, 2, 2}, // DIRT
    {3, 3, 3}, // SAND
    {6, 6, 6}, // WATER
    {5, 5, 5}, // OAKTREETRUNK
    {4, 4, 4}, // OAKLEAF
};

static inline u8 calcAO(u8 s1, u8 s2, u8 co) {
    s1 = s1 != AIR && s1 != WATER;
    s2 = s2 != AIR && s2 != WATER;
    co = co != AIR && co != WATER;
    if (s1 && s2)
        return 0;
    return 3 - (s1 + s2 + co);
}

static inline u32 pack(u16 x, u16 y, u16 z, u8 n, u8 uv, u8 ao, u8 t)
{
    u32 r = ((x  & ONES(4)) <<  0) |
            ((z  & ONES(4)) <<  4) |
            ((y  & ONES(8)) <<  8) |
            ((n  & ONES(3)) << 16) |
            ((uv & ONES(2)) << 19) |
            ((ao & ONES(2)) << 21) |
            ((t  & ONES(8)) << 23) ;
    return r;
}

enum {
    _NEG_ = 1 << 2,
    _X_   = 0,
    _Y_   = 1,
    _Z_   = 2,

    N_WST = _NEG_ | _X_,
    N_EST = _X_,
    N_BOT = _NEG_ | _Y_,
    N_TOP = _Y_,
    N_SOU = _NEG_ | _Z_,
    N_NOR = _Z_,
};

void fillVerts(u32 *verts, u32 &count, u16 x, u16 y, u16 z, u8 c, const Surrounding &su)
{
    ASSERT(c != AIR && c < _BLOCK_TYPE_MAX_, "invalid block type");
    u8 t = blockIndex[c].t;
    u8 s = blockIndex[c].s;
    u8 b = blockIndex[c].b;
    u8 a0, a1, a2, a3;

    // SOUTH
    if (su.ms == AIR || (su.ms == WATER && c != WATER)) {
        a0 = calcAO(su.bs, su.msw, su.bsw);
        a1 = calcAO(su.ts, su.msw, su.tsw);
        a2 = calcAO(su.bs, su.mse, su.bse);
        a3 = calcAO(su.ts, su.mse, su.tse);
        if (a0 * a3 < a1 * a2) {
            verts[count++] = pack(x + 1, y + 0, z + 0, N_SOU, 3, a2, s);
            verts[count++] = pack(x + 0, y + 0, z + 0, N_SOU, 1, a0, s);
            verts[count++] = pack(x + 0, y + 1, z + 0, N_SOU, 0, a1, s);
            verts[count++] = pack(x + 1, y + 0, z + 0, N_SOU, 3, a2, s);
            verts[count++] = pack(x + 0, y + 1, z + 0, N_SOU, 0, a1, s);
            verts[count++] = pack(x + 1, y + 1, z + 0, N_SOU, 2, a3, s);
        } else {
            verts[count++] = pack(x + 0, y + 0, z + 0, N_SOU, 1, a0, s);
            verts[count++] = pack(x + 0, y + 1, z + 0, N_SOU, 0, a1, s);
            verts[count++] = pack(x + 1, y + 1, z + 0, N_SOU, 2, a3, s);
            verts[count++] = pack(x + 0, y + 0, z + 0, N_SOU, 1, a0, s);
            verts[count++] = pack(x + 1, y + 1, z + 0, N_SOU, 2, a3, s);
            verts[count++] = pack(x + 1, y + 0, z + 0, N_SOU, 3, a2, s);
        }
    }

    // NORTH
    if (su.mn == AIR || (su.mn == WATER && c != WATER)) {
        a0 = calcAO(su.bn, su.mne, su.bne);
        a1 = calcAO(su.tn, su.mne, su.tne);
        a2 = calcAO(su.bn, su.mnw, su.bnw);
        a3 = calcAO(su.tn, su.mnw, su.tnw);
        if (a0 * a3 < a1 * a2) {
            verts[count++] = pack(x + 0, y + 0, z + 1, N_NOR, 1, a2, s);
            verts[count++] = pack(x + 1, y + 0, z + 1, N_NOR, 3, a0, s);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_NOR, 2, a1, s);
            verts[count++] = pack(x + 0, y + 0, z + 1, N_NOR, 1, a2, s);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_NOR, 2, a1, s);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_NOR, 0, a3, s);
        } else {
            verts[count++] = pack(x + 1, y + 0, z + 1, N_NOR, 3, a0, s);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_NOR, 2, a1, s);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_NOR, 0, a3, s);
            verts[count++] = pack(x + 1, y + 0, z + 1, N_NOR, 3, a0, s);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_NOR, 0, a3, s);
            verts[count++] = pack(x + 0, y + 0, z + 1, N_NOR, 1, a2, s);
        }
    }

    // EAST
    if (su.me == AIR || (su.me == WATER && c != WATER)) {
        a0 = calcAO(su.be, su.mse, su.bse);
        a1 = calcAO(su.te, su.mse, su.tse);
        a2 = calcAO(su.be, su.mne, su.bne);
        a3 = calcAO(su.te, su.mne, su.tne);
        if (a0 * a3 < a1 * a2) {
            verts[count++] = pack(x + 1, y + 0, z + 1, N_EST, 3, a2, s);
            verts[count++] = pack(x + 1, y + 0, z + 0, N_EST, 1, a0, s);
            verts[count++] = pack(x + 1, y + 1, z + 0, N_EST, 0, a1, s);
            verts[count++] = pack(x + 1, y + 0, z + 1, N_EST, 3, a2, s);
            verts[count++] = pack(x + 1, y + 1, z + 0, N_EST, 0, a1, s);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_EST, 2, a3, s);
        } else {
            verts[count++] = pack(x + 1, y + 0, z + 0, N_EST, 1, a0, s);
            verts[count++] = pack(x + 1, y + 1, z + 0, N_EST, 0, a1, s);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_EST, 2, a3, s);
            verts[count++] = pack(x + 1, y + 0, z + 0, N_EST, 1, a0, s);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_EST, 2, a3, s);
            verts[count++] = pack(x + 1, y + 0, z + 1, N_EST, 3, a2, s);
        }
    }

    // WEST
    if (su.mw == AIR || (su.mw == WATER && c != WATER)) {
        a0 = calcAO(su.bw, su.mnw, su.bnw);
        a1 = calcAO(su.tw, su.mnw, su.tnw);
        a2 = calcAO(su.bw, su.msw, su.bsw);
        a3 = calcAO(su.tw, su.msw, su.tsw);
        if (a0 * a3 < a1 * a2) {
            verts[count++] = pack(x + 0, y + 0, z + 0, N_WST, 1, a2, s);
            verts[count++] = pack(x + 0, y + 0, z + 1, N_WST, 3, a0, s);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_WST, 2, a1, s);
            verts[count++] = pack(x + 0, y + 0, z + 0, N_WST, 1, a2, s);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_WST, 2, a1, s);
            verts[count++] = pack(x + 0, y + 1, z + 0, N_WST, 0, a3, s);
        } else {
            verts[count++] = pack(x + 0, y + 0, z + 1, N_WST, 3, a0, s);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_WST, 2, a1, s);
            verts[count++] = pack(x + 0, y + 1, z + 0, N_WST, 0, a3, s);
            verts[count++] = pack(x + 0, y + 0, z + 1, N_WST, 3, a0, s);
            verts[count++] = pack(x + 0, y + 1, z + 0, N_WST, 0, a3, s);
            verts[count++] = pack(x + 0, y + 0, z + 0, N_WST, 1, a2, s);
        }
    }

    // TOP
    if (su.t  == AIR || (su.t  == WATER && c != WATER)) {
        a0 = calcAO(su.tw, su.ts, su.tsw);
        a1 = calcAO(su.tw, su.tn, su.tnw);
        a2 = calcAO(su.te, su.ts, su.tse);
        a3 = calcAO(su.te, su.tn, su.tne);
        if (a0 * a3 < a1 * a2) {
            verts[count++] = pack(x + 1, y + 1, z + 0, N_TOP, 2, a2, t);
            verts[count++] = pack(x + 0, y + 1, z + 0, N_TOP, 0, a0, t);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_TOP, 1, a1, t);
            verts[count++] = pack(x + 1, y + 1, z + 0, N_TOP, 2, a2, t);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_TOP, 1, a1, t);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_TOP, 3, a3, t);
        } else {
            verts[count++] = pack(x + 0, y + 1, z + 0, N_TOP, 0, a0, t);
            verts[count++] = pack(x + 0, y + 1, z + 1, N_TOP, 1, a1, t);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_TOP, 3, a3, t);
            verts[count++] = pack(x + 0, y + 1, z + 0, N_TOP, 0, a0, t);
            verts[count++] = pack(x + 1, y + 1, z + 1, N_TOP, 3, a3, t);
            verts[count++] = pack(x + 1, y + 1, z + 0, N_TOP, 2, a2, t);
        }
    }

    // BOTTOM
    if (su.b  == AIR || (su.b  == WATER && c != WATER)) {
        a0 = calcAO(su.be, su.bn, su.bne);
        a1 = calcAO(su.bw, su.bn, su.bnw);
        a2 = calcAO(su.be, su.bs, su.bse);
        a3 = calcAO(su.bw, su.bs, su.bsw);
        if (a0 * a3 < a1 * a2) {
            verts[count++] = pack(x + 1, y + 0, z + 0, N_BOT, 2, a2, b);
            verts[count++] = pack(x + 1, y + 0, z + 1, N_BOT, 3, a0, b);
            verts[count++] = pack(x + 0, y + 0, z + 1, N_BOT, 1, a1, b);
            verts[count++] = pack(x + 1, y + 0, z + 0, N_BOT, 2, a2, b);
            verts[count++] = pack(x + 0, y + 0, z + 1, N_BOT, 1, a1, b);
            verts[count++] = pack(x + 0, y + 0, z + 0, N_BOT, 0, a3, b);
        } else {
            verts[count++] = pack(x + 1, y + 0, z + 1, N_BOT, 3, a0, b);
            verts[count++] = pack(x + 0, y + 0, z + 1, N_BOT, 1, a1, b);
            verts[count++] = pack(x + 0, y + 0, z + 0, N_BOT, 0, a3, b);
            verts[count++] = pack(x + 1, y + 0, z + 1, N_BOT, 3, a0, b);
            verts[count++] = pack(x + 0, y + 0, z + 0, N_BOT, 0, a3, b);
            verts[count++] = pack(x + 1, y + 0, z + 0, N_BOT, 2, a2, b);
        }
    }
}
