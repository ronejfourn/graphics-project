#include "block.hpp"
#define ONES(n) ((1 << n) - 1)

static_assert(_BLOCK_TYPE_MAX_ < 256, "too many block types");

static struct {
    u8 t, s, b;
} blockIndex[_BLOCK_TYPE_MAX_] = {
    {       }, // AIR
    {0, 1, 2}, // GRASS
    {3, 3, 3}, // WATER
};

static inline u32 pack(u16 x, u16 y, u16 z, u8 uv, u8 t)
{
    u32 r = ((x  & ONES(5)) <<  0) |
            ((z  & ONES(5)) <<  5) |
            ((y  & ONES(9)) << 10) |
            ((uv & ONES(2)) << 19) |
            ((t           ) << 21) ;
    return r;
}

void fillVerts(u32 *verts, u32 &count, u16 x, u16 y, u16 z, u8 type,
        u8 sb, u8 nb, u8 eb, u8 wb, u8 tb, u8 bb)
{
    ASSERT(type < _BLOCK_TYPE_MAX_, "invalid block type");

    u8 t = blockIndex[type].t;
    u8 s = blockIndex[type].s;
    u8 b = blockIndex[type].b;

    // SOUTH
    if (sb == AIR) {
        verts[count++] = pack(x + 0, y + 0, z + 0, 1, s);
        verts[count++] = pack(x + 0, y + 1, z + 0, 0, s);
        verts[count++] = pack(x + 1, y + 1, z + 0, 2, s);
        verts[count++] = pack(x + 0, y + 0, z + 0, 1, s);
        verts[count++] = pack(x + 1, y + 1, z + 0, 2, s);
        verts[count++] = pack(x + 1, y + 0, z + 0, 3, s);
    }

    // NORTH
    if (nb == AIR) {
        verts[count++] = pack(x + 1, y + 0, z + 1, 3, s);
        verts[count++] = pack(x + 1, y + 1, z + 1, 2, s);
        verts[count++] = pack(x + 0, y + 1, z + 1, 0, s);
        verts[count++] = pack(x + 1, y + 0, z + 1, 3, s);
        verts[count++] = pack(x + 0, y + 1, z + 1, 0, s);
        verts[count++] = pack(x + 0, y + 0, z + 1, 1, s);
    }

    // EAST
    if (eb == AIR) {
        verts[count++] = pack(x + 1, y + 0, z + 0, 1, s);
        verts[count++] = pack(x + 1, y + 1, z + 0, 0, s);
        verts[count++] = pack(x + 1, y + 1, z + 1, 2, s);
        verts[count++] = pack(x + 1, y + 0, z + 0, 1, s);
        verts[count++] = pack(x + 1, y + 1, z + 1, 2, s);
        verts[count++] = pack(x + 1, y + 0, z + 1, 3, s);
    }

    // WEST
    if (wb == AIR) {
        verts[count++] = pack(x + 0, y + 0, z + 1, 3, s);
        verts[count++] = pack(x + 0, y + 1, z + 1, 2, s);
        verts[count++] = pack(x + 0, y + 1, z + 0, 0, s);
        verts[count++] = pack(x + 0, y + 0, z + 1, 3, s);
        verts[count++] = pack(x + 0, y + 1, z + 0, 0, s);
        verts[count++] = pack(x + 0, y + 0, z + 0, 1, s);
    }

    // TOP
    if (tb == AIR) {
        verts[count++] = pack(x + 0, y + 1, z + 0, 0, t);
        verts[count++] = pack(x + 0, y + 1, z + 1, 1, t);
        verts[count++] = pack(x + 1, y + 1, z + 1, 3, t);
        verts[count++] = pack(x + 0, y + 1, z + 0, 0, t);
        verts[count++] = pack(x + 1, y + 1, z + 1, 3, t);
        verts[count++] = pack(x + 1, y + 1, z + 0, 2, t);
    }

    // BOTTOM
    if (bb == AIR) {
        verts[count++] = pack(x + 1, y + 0, z + 1, 3, b);
        verts[count++] = pack(x + 0, y + 0, z + 1, 1, b);
        verts[count++] = pack(x + 0, y + 0, z + 0, 0, b);
        verts[count++] = pack(x + 1, y + 0, z + 1, 3, b);
        verts[count++] = pack(x + 0, y + 0, z + 0, 0, b);
        verts[count++] = pack(x + 1, y + 0, z + 0, 2, b);
    }
}
