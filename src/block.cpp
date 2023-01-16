#include "block.hpp"

#define ONES(n) ((1 << n) - 1)

static inline u32 pack(u16 x, u16 y, u16 z, u8 t)
{
    u32 r = ((x & ONES(5)) <<  0) |
            ((z & ONES(5)) <<  5) |
            ((y & ONES(9)) << 10) |
            ((t          ) << 19) ;
    return r;
}

void fillVerts(u32 *verts, u32 &count, u16 x, u16 y, u16 z, u8 type,
        u8 sb, u8 nb, u8 eb, u8 wb, u8 tb, u8 bb)
{
    u8 t = 0x2;
    u8 s = 0x0;
    u8 b = 0x7;

    if (type == 2) {t = s = b = 0x4;}

    // SOUTH
    if (sb == 0) {
        verts[count++] = pack(x + 0, y + 0, z + 0, s);
        verts[count++] = pack(x + 0, y + 1, z + 0, s);
        verts[count++] = pack(x + 1, y + 1, z + 0, s);
        verts[count++] = pack(x + 0, y + 0, z + 0, s);
        verts[count++] = pack(x + 1, y + 1, z + 0, s);
        verts[count++] = pack(x + 1, y + 0, z + 0, s);
    }

    // NORTH
    if (nb == 0) {
        verts[count++] = pack(x + 1, y + 0, z + 1, s);
        verts[count++] = pack(x + 1, y + 1, z + 1, s);
        verts[count++] = pack(x + 0, y + 1, z + 1, s);
        verts[count++] = pack(x + 1, y + 0, z + 1, s);
        verts[count++] = pack(x + 0, y + 1, z + 1, s);
        verts[count++] = pack(x + 0, y + 0, z + 1, s);
    }

    // EAST
    if (eb == 0) {
        verts[count++] = pack(x + 1, y + 0, z + 0, s);
        verts[count++] = pack(x + 1, y + 1, z + 0, s);
        verts[count++] = pack(x + 1, y + 1, z + 1, s);
        verts[count++] = pack(x + 1, y + 0, z + 0, s);
        verts[count++] = pack(x + 1, y + 1, z + 1, s);
        verts[count++] = pack(x + 1, y + 0, z + 1, s);
    }

    // WEST
    if (wb == 0) {
        verts[count++] = pack(x + 0, y + 0, z + 1, s);
        verts[count++] = pack(x + 0, y + 1, z + 1, s);
        verts[count++] = pack(x + 0, y + 1, z + 0, s);
        verts[count++] = pack(x + 0, y + 0, z + 1, s);
        verts[count++] = pack(x + 0, y + 1, z + 0, s);
        verts[count++] = pack(x + 0, y + 0, z + 0, s);
    }

    // TOP
    if (tb == 0) {
        verts[count++] = pack(x + 0, y + 1, z + 0, t);
        verts[count++] = pack(x + 0, y + 1, z + 1, t);
        verts[count++] = pack(x + 1, y + 1, z + 1, t);
        verts[count++] = pack(x + 0, y + 1, z + 0, t);
        verts[count++] = pack(x + 1, y + 1, z + 1, t);
        verts[count++] = pack(x + 1, y + 1, z + 0, t);
    }

    // BOTTOM
    if (bb == 0) {
        verts[count++] = pack(x + 1, y + 0, z + 1, b);
        verts[count++] = pack(x + 0, y + 0, z + 1, b);
        verts[count++] = pack(x + 0, y + 0, z + 0, b);
        verts[count++] = pack(x + 1, y + 0, z + 1, b);
        verts[count++] = pack(x + 0, y + 0, z + 0, b);
        verts[count++] = pack(x + 1, y + 0, z + 0, b);
    }
}
