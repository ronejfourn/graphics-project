#include "glad/glad.h"
#include "rendering/shader.hpp"
#include "world/chunk.hpp"
#include "world/block.hpp"
#include "utility/noise.hpp"

#include <cstdio>
#include <math.h>
#include <memory.h>

static constexpr i32 seaLevel      = 65;
static constexpr  u8 baseHeight    = 45;
static constexpr  u8 maxHeight     = 150;

Chunk::Chunk() :
    m_vao(DYNAMIC)
{
    memset(m_blocks, AIR, sizeof(m_blocks));

    m_east      = Chunk::dummy();
    m_west      = Chunk::dummy();
    m_north     = Chunk::dummy();
    m_south     = Chunk::dummy();
    m_northeast = Chunk::dummy();
    m_southeast = Chunk::dummy();
    m_northwest = Chunk::dummy();
    m_southwest = Chunk::dummy();

    m_opaquevertcount = 0;
    m_transparentvertcount = 0;
    m_state = Initial;

    m_vao.bind();
    VertexAttrib va = {0, 1, UINT};
    m_vao.setAttribs(1, &va);
}

Chunk::Chunk(u8 t) { memset(m_blocks, t, sizeof(m_blocks)); }
void Chunk::setEast     (Chunk *p) { m_east      = p; m_state = NeedsUpdating; }
void Chunk::setWest     (Chunk *p) { m_west      = p; m_state = NeedsUpdating; }
void Chunk::setNorth    (Chunk *p) { m_north     = p; m_state = NeedsUpdating; }
void Chunk::setSouth    (Chunk *p) { m_south     = p; m_state = NeedsUpdating; }
void Chunk::setNorthEast(Chunk *p) { m_northeast = p; m_state = NeedsUpdating; }
void Chunk::setSouthEast(Chunk *p) { m_southeast = p; m_state = NeedsUpdating; }
void Chunk::setNorthWest(Chunk *p) { m_northwest = p; m_state = NeedsUpdating; }
void Chunk::setSouthWest(Chunk *p) { m_southwest = p; m_state = NeedsUpdating; }

bool Chunk::_checkForOakTree(i32 x, i32 y, i32 z)
{
    if (x == 7 && z == 7 &&
        y > seaLevel &&
        m_blocks[x][z][y] == GRASS)
        return true;
    else
        return false;
}

void Chunk::_placeOakTree(i32 x, i32 y, i32 z, pcg32_random_t *rng)
{
    u32 treeHeight;//Sets the max height of the tree
    u32 randomHeight = (pcg32_random_r(rng) & 7);

    treeHeight = 6 + randomHeight;
    u32 count  = 0;
    u32 max    = 3 + randomHeight;
    u32 ymax = y + treeHeight - 1;

    auto placeLeaf = [this](i32 y, i32 x, i32 z) {
        if (m_blocks[x][z][y] == AIR)
            m_blocks[x][z][y] = OAKLEAF;
    };

    auto placeLeaves = [placeLeaf](i32 cx, i32 y, i32 cz, i8 r) {
        i32 p = 1 - r;
        i32 x = 0;
        i32 z = r;

        while (x <= z) {
            for (i32 i = -z; i <= +z; i ++) {
                placeLeaf(y, cx + x, cz + i);
                placeLeaf(y, cx - x, cz + i);
                placeLeaf(y, cx + i, cx + x);
                placeLeaf(y, cx - i, cx - x);
            }

            p = p + (2 * (++x)) + 1;
            if (p >= 0)
                p = p - (2 * (--z));
        }
    };

    for (i32 k = ymax; k >= y; k--) {
        m_blocks[x][z][k] = OAKTREETRUNK;
        if((count ++) < max) {
            u32 radius = count / 2 + 2;
            placeLeaves(x, k, z, radius);
        }
    }

    m_blocks[x][z][y - 1] = DIRT;
    m_blocks[x][z][y + treeHeight] = OAKLEAF;
}

void Chunk::generate(i32 x, i32 z, FBMConfig& fc)
{
    pcg32_random_t rng = { (((u64)x * 3452189327901ull + 12682897369ull) * ((u64)z * 129728736478123ull + 1987724839021ull)) | 1, 32874012398623949ull };

    m_state = NeedsUpdating;

    m_origin = Vec3((f32)x * CHUNK_MAX_X, 0, (f32)z * CHUNK_MAX_Z);
    bool hasTree = false;
    memset(m_blocks, AIR, sizeof(m_blocks));

    for (u8 cx = 0; cx < CHUNK_MAX_X; cx++) {
        for (u8 cz = 0; cz < CHUNK_MAX_Z; cz++)  {
            f32 n = noise((x + cx / (f32)CHUNK_MAX_X) / 32.0f, (z + cz / (f32)CHUNK_MAX_Z) / 32.0f, fc.permutation) * 0.5 + 0.5;
            u8 height = (u8)(fbm(x + cx / (f32)CHUNK_MAX_X, z + cz / (f32)CHUNK_MAX_Z, fc)
                * maxHeight * n + baseHeight);

            if (height > seaLevel) {
                m_blocks[cx][cz][height - 1] = GRASS;
                for (i32 y = 0; y < height - 1; y++)
                    m_blocks[cx][cz][y] = DIRT;

                for (i32 y = seaLevel - 5; y <= seaLevel + 5; y++) {
                    i32 d = y - seaLevel + 1;
                    d = d < 0 ? -d : d;
                    auto &cur = m_blocks[cx][cz][y];
                    if ((cur == GRASS || cur == DIRT) && ((pcg32_random_r(&rng) & ((1 << d) - 1)) <= d))
                        cur = SAND;
                }

                if (!hasTree && _checkForOakTree(cx, height - 1, cz)) {
                    if (!(pcg32_random_r(&rng) & 7)) {
                        hasTree = true;
                        _placeOakTree(cx, height, cz, &rng);
                    }
                }
            } else {
                for (i32 y = height; y < seaLevel; y++)
                    m_blocks[cx][cz][y] = WATER;
                for (i32 y = 0; y < height; y++)
                    m_blocks[cx][cz][y] = SAND;
            }
        }
    }
}

void Chunk::renderPrep(const Shader &shader)
{
    shader.uniform("xz", m_renderOrigin.x, m_renderOrigin.z);
    m_vao.bind();
}

void Chunk::renderOpaque(const Shader &shader)
{
    if (!m_opaquevertcount) return;
    glDrawArrays(GL_TRIANGLES, 0, m_opaquevertcount);
}

void Chunk::renderTransparent(const Shader &shader)
{
    if (!m_transparentvertcount) return;
    glEnable(GL_BLEND);
    glDrawArrays(GL_TRIANGLES, m_opaquevertcount, m_transparentvertcount);
    glDisable(GL_BLEND);
}

static constexpr u32 maxVertCount = CHUNK_MAX_X * CHUNK_MAX_Y * CHUNK_MAX_Z * 6 * 6;
static u32 opaqueverts[maxVertCount];
static u32 transparentverts[maxVertCount];

void Chunk::update()
{
    m_state = Ready;
    m_renderOrigin = m_origin;
    m_opaquevertcount = 0;
    m_transparentvertcount = 0;
    static constexpr u32 XMIN = 0;
    static constexpr u32 ZMIN = 0;
    static constexpr u32 XMAX = CHUNK_MAX_X - 1;
    static constexpr u32 ZMAX = CHUNK_MAX_Z - 1;
    static constexpr u32 YMAX = CHUNK_MAX_Y - 1;
    const u8 *e, *w, *n, *s, *ne, *nw, *se, *sw;

    for (u16 x = XMIN; x <= XMAX; x ++) {
        for (u16 z = ZMIN; z <= ZMAX; z ++) {
            const u8 *c  = m_blocks[x][z];

            if (x == XMIN) {
                se = m_blocks[x + 1][z - 1];
                e  = m_blocks[x + 1][z];
                ne = m_blocks[x + 1][z + 1];
                sw = m_west->m_blocks[XMAX][z - 1];
                w  = m_west->m_blocks[XMAX][z];
                nw = m_west->m_blocks[XMAX][z + 1];
            } else if (x == XMAX) {
                sw = m_blocks[x - 1][z - 1];
                w  = m_blocks[x - 1][z];
                nw = m_blocks[x - 1][z + 1];
                se = m_east->m_blocks[XMIN][z - 1];
                e  = m_east->m_blocks[XMIN][z];
                ne = m_east->m_blocks[XMIN][z + 1];
            } else {
                se = m_blocks[x + 1][z - 1];
                e  = m_blocks[x + 1][z];
                ne = m_blocks[x + 1][z + 1];
                sw = m_blocks[x - 1][z - 1];
                w  = m_blocks[x - 1][z];
                nw = m_blocks[x - 1][z + 1];
            }

            s = m_blocks[x][z - 1];
            n = m_blocks[x][z + 1];

            if (z == ZMIN) {
                s = m_south->m_blocks[x][ZMAX];
                if (x == XMIN) {
                    se = m_south->m_blocks[x + 1][ZMAX];
                    sw = m_southwest->m_blocks[XMAX][ZMAX];
                } else if (x == XMAX) {
                    sw = m_south->m_blocks[x - 1][ZMAX];
                    se = m_southeast->m_blocks[XMIN][ZMAX];
                } else {
                    se = m_south->m_blocks[x + 1][ZMAX];
                    sw = m_south->m_blocks[x - 1][ZMAX];
                }
            } else if (z == ZMAX) {
                n = m_north->m_blocks[x][ZMIN];
                if (x == XMIN) {
                    ne = m_north->m_blocks[x + 1][ZMIN];
                    nw = m_northwest->m_blocks[XMAX][ZMIN];
                } else if (x == XMAX) {
                    nw = m_north->m_blocks[x - 1][ZMIN];
                    ne = m_northeast->m_blocks[XMIN][ZMIN];
                } else {
                    ne = m_north->m_blocks[x + 1][ZMIN];
                    nw = m_north->m_blocks[x - 1][ZMIN];
                }
            }

            u8 t = c[0];
            if (t != AIR) {
                Surrounding su = {};
                su.t   =  c[1];
                su.me  =  e[0]; su.te  =  e[1];
                su.mw  =  w[0]; su.tw  =  w[1];
                su.mn  =  n[0]; su.tn  =  n[1];
                su.ms  =  s[0]; su.ts  =  s[1];
                su.mne = ne[0]; su.tne = ne[1];
                su.mnw = nw[0]; su.tnw = nw[1];
                su.mse = se[0]; su.tse = se[1];
                su.msw = sw[0]; su.tsw = sw[1];
                (t != WATER) ?
                    fillVerts(opaqueverts, m_opaquevertcount, x, 0, z, t, su):
                    fillVerts(transparentverts, m_transparentvertcount, x, 0, z, t, su);
            }

            for (u16 y = 1; y < CHUNK_MAX_Y - 1; y ++) {
                t = c[y];
                if (t == AIR) continue;
                Surrounding su = {};
                su.b   =  c[y - 1];                 su.t   =  c[y + 1];
                su.be  =  e[y - 1]; su.me  =  e[y]; su.te  =  e[y + 1];
                su.bw  =  w[y - 1]; su.mw  =  w[y]; su.tw  =  w[y + 1];
                su.bn  =  n[y - 1]; su.mn  =  n[y]; su.tn  =  n[y + 1];
                su.bs  =  s[y - 1]; su.ms  =  s[y]; su.ts  =  s[y + 1];
                su.bne = ne[y - 1]; su.mne = ne[y]; su.tne = ne[y + 1];
                su.bnw = nw[y - 1]; su.mnw = nw[y]; su.tnw = nw[y + 1];
                su.bse = se[y - 1]; su.mse = se[y]; su.tse = se[y + 1];
                su.bsw = sw[y - 1]; su.msw = sw[y]; su.tsw = sw[y + 1];
                (t != WATER) ?
                    fillVerts(opaqueverts, m_opaquevertcount, x, y, z, t, su):
                    fillVerts(transparentverts, m_transparentvertcount, x, y, z, t, su);
            }

            t = c[YMAX];
            if (t != AIR) {
                Surrounding su = {};
                su.b   =  c[YMAX - 1];
                su.me  =  e[YMAX - 0]; su.be  =  e[YMAX - 1];
                su.mw  =  w[YMAX - 0]; su.bw  =  w[YMAX - 1];
                su.mn  =  n[YMAX - 0]; su.bn  =  n[YMAX - 1];
                su.ms  =  s[YMAX - 0]; su.bs  =  s[YMAX - 1];
                su.mne = ne[YMAX - 0]; su.bne = ne[YMAX - 1];
                su.mnw = nw[YMAX - 0]; su.bnw = nw[YMAX - 1];
                su.mse = se[YMAX - 0]; su.bse = se[YMAX - 1];
                su.msw = sw[YMAX - 0]; su.bsw = sw[YMAX - 1];
                (t != WATER) ?
                    fillVerts(opaqueverts, m_opaquevertcount, x, YMAX, z, t, su):
                    fillVerts(transparentverts, m_transparentvertcount, x, YMAX, z, t, su);
            }
        }
    }

    auto count = m_transparentvertcount + m_opaquevertcount;
    if (count) {
        m_vao.bind();
        if (!m_opaquevertcount) {
            m_vao.setData(count * 4, transparentverts);
        } else if (!m_transparentvertcount) {
            m_vao.setData(count * 4, opaqueverts);
        } else {
            m_vao.setData(count * 4, opaqueverts);
            m_vao.subData(m_transparentvertcount * 4, transparentverts, m_opaquevertcount * 4);
        }
    }
}
