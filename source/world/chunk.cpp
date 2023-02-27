#include "glad/glad.h"
#include "rendering/shader.hpp"
#include "world/chunk.hpp"
#include "world/block.hpp"
#include "utility/noise.hpp"

#include <math.h>
#include <memory.h>

static constexpr i32 seaLevel      = 65;
static constexpr  u8 baseHeight    = 45;
static constexpr  u8 maxHeight     = 150;

Chunk::Chunk() :
    m_vao(DYNAMIC)
{
    memset(m_blocks, AIR, sizeof(m_blocks));

    m_east      = s_dummy();
    m_west      = s_dummy();
    m_north     = s_dummy();
    m_south     = s_dummy();
    m_northeast = s_dummy();
    m_southeast = s_dummy();
    m_northwest = s_dummy();
    m_southwest = s_dummy();

    m_opaquevertcount = 0;
    m_transparentvertcount = 0;
    m_state = Initial;

    m_vao.bind();
    VertexAttrib va = {0, 1, UINT};
    m_vao.setAttribs(1, &va);
}

void Chunk::resetNeighbours() {
    m_east     ->m_west      = s_dummy(); m_east     ->m_state = NeedsUpdating;
    m_west     ->m_east      = s_dummy(); m_west     ->m_state = NeedsUpdating;
    m_north    ->m_south     = s_dummy(); m_north    ->m_state = NeedsUpdating;
    m_south    ->m_north     = s_dummy(); m_south    ->m_state = NeedsUpdating;
    m_northeast->m_southwest = s_dummy(); m_northeast->m_state = NeedsUpdating;
    m_southeast->m_northwest = s_dummy(); m_southeast->m_state = NeedsUpdating;
    m_northwest->m_southeast = s_dummy(); m_northwest->m_state = NeedsUpdating;
    m_southwest->m_northeast = s_dummy(); m_southwest->m_state = NeedsUpdating;

    m_east      = s_dummy();
    m_west      = s_dummy();
    m_north     = s_dummy();
    m_south     = s_dummy();
    m_northeast = s_dummy();
    m_southeast = s_dummy();
    m_northwest = s_dummy();
    m_southwest = s_dummy();
    m_state = NeedsUpdating;
}

Chunk::Chunk(u8 t) { memset(m_blocks, t, sizeof(m_blocks)); }
void Chunk::setEast     (Chunk *p) { m_east      = p; p->m_west      = this; m_state = NeedsUpdating; p->m_state = NeedsUpdating; }
void Chunk::setWest     (Chunk *p) { m_west      = p; p->m_east      = this; m_state = NeedsUpdating; p->m_state = NeedsUpdating; }
void Chunk::setNorth    (Chunk *p) { m_north     = p; p->m_south     = this; m_state = NeedsUpdating; p->m_state = NeedsUpdating; }
void Chunk::setSouth    (Chunk *p) { m_south     = p; p->m_north     = this; m_state = NeedsUpdating; p->m_state = NeedsUpdating; }
void Chunk::setNorthEast(Chunk *p) { m_northeast = p; p->m_southwest = this; m_state = NeedsUpdating; p->m_state = NeedsUpdating; }
void Chunk::setSouthEast(Chunk *p) { m_southeast = p; p->m_northwest = this; m_state = NeedsUpdating; p->m_state = NeedsUpdating; }
void Chunk::setNorthWest(Chunk *p) { m_northwest = p; p->m_southeast = this; m_state = NeedsUpdating; p->m_state = NeedsUpdating; }
void Chunk::setSouthWest(Chunk *p) { m_southwest = p; p->m_northeast = this; m_state = NeedsUpdating; p->m_state = NeedsUpdating; }

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
    m_center = {m_origin.x + CHUNK_MAX_X / 2.0f, CHUNK_MAX_Y / 2.0f, m_origin.z + CHUNK_MAX_Z / 2.0f};
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
                    if ((cur == GRASS || cur == DIRT) && ((pcg32_random_r(&rng) & (u32)(((1 << d) - 1) <= d))))
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

void Chunk::renderOpaque()
{
    if (!m_opaquevertcount) return;
    glDrawArrays(GL_TRIANGLES, 0, m_opaquevertcount);
}

void Chunk::renderTransparent()
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
    static constexpr u32 XMAX = CHUNK_MAX_X - 1;
    static constexpr u32 ZMAX = CHUNK_MAX_Z - 1;
    static constexpr u32 YMAX = CHUNK_MAX_Y - 1;
    const u8 *c, *e, *w, *n, *s, *ne, *nw, *se, *sw;
    const u8 (*nne)[CHUNK_MAX_Y];
    const u8 (*nnw)[CHUNK_MAX_Y];

    for (u32 x = 0; x <= XMAX; x ++) {
        c = m_south->m_blocks[x][ZMAX];
        n = m_blocks[x][0];

        if (x == 0) {
            nnw = m_west->m_blocks[XMAX];
            nne = m_blocks[x + 1];
            w = m_southwest->m_blocks[XMAX][ZMAX];
            e = m_south->m_blocks[x + 1][ZMAX];
        } else if (x == XMAX) {
            nnw = m_blocks[x - 1];
            nne = m_east->m_blocks[0];
            w = m_south->m_blocks[x - 1][ZMAX];
            e = m_southeast->m_blocks[0][ZMAX];
        } else {
            nnw = m_blocks[x - 1];
            nne = m_blocks[x + 1];
            w = m_south->m_blocks[x - 1][ZMAX];
            e = m_south->m_blocks[x + 1][ZMAX];
        }

        ne = nne[0];
        nw = nnw[0];

        for (u32 z = 0; z <= ZMAX; z ++) {
            se = e, e = ne;
            s  = c, c = n;
            sw = w, w = nw;

            if (z == ZMAX) {
                n = m_north->m_blocks[x][0];
                if (x == 0) {
                    nw = m_northwest->m_blocks[XMAX][0];
                    ne = m_north->m_blocks[x + 1][0];
                } else if (x == XMAX) {
                    nw = m_north->m_blocks[x - 1][0];
                    ne = m_northeast->m_blocks[0][0];
                } else {
                    nw = m_north->m_blocks[x - 1][0];
                    ne = m_north->m_blocks[x + 1][0];
                }
            } else {
                n  = m_blocks[x][z + 1];
                ne = nne[z + 1];
                nw = nnw[z + 1];
            }

            u8 curr = AIR;
            Surrounding su = { };
            su.t   =  c[0], su.te  =  e[0], su.tw  =  w[0],
            su.tn  =  n[0], su.ts  =  s[0], su.tne = ne[0],
            su.tnw = nw[0], su.tse = se[0], su.tsw = sw[0];

            u32 y;
            for (y = 0; y < YMAX; y ++) {
                su.b   = curr;
                curr   = c[y];
                su.t   = c[y + 1];
                su.be  = su.me ; su.me  = su.te ; su.te  =  e[y + 1];
                su.bw  = su.mw ; su.mw  = su.tw ; su.tw  =  w[y + 1];
                su.bn  = su.mn ; su.mn  = su.tn ; su.tn  =  n[y + 1];
                su.bs  = su.ms ; su.ms  = su.ts ; su.ts  =  s[y + 1];
                su.bne = su.mne; su.mne = su.tne; su.tne = ne[y + 1];
                su.bnw = su.mnw; su.mnw = su.tnw; su.tnw = nw[y + 1];
                su.bse = su.mse; su.mse = su.tse; su.tse = se[y + 1];
                su.bsw = su.msw; su.msw = su.tsw; su.tsw = sw[y + 1];

                if (curr == WATER) fillVerts(transparentverts, m_transparentvertcount, x, y, z, curr, su);
                else if (curr != AIR) fillVerts(opaqueverts, m_opaquevertcount, x, y, z, curr, su);
            }

            su.b   = curr;
            curr   = c[y];
            su.t   = AIR;
            su.be  = su.me ; su.me  = su.te ; su.te  = AIR;
            su.bw  = su.mw ; su.mw  = su.tw ; su.tw  = AIR;
            su.bn  = su.mn ; su.mn  = su.tn ; su.tn  = AIR;
            su.bs  = su.ms ; su.ms  = su.ts ; su.ts  = AIR;
            su.bne = su.mne; su.mne = su.tne; su.tne = AIR;
            su.bnw = su.mnw; su.mnw = su.tnw; su.tnw = AIR;
            su.bse = su.mse; su.mse = su.tse; su.tse = AIR;
            su.bsw = su.msw; su.msw = su.tsw; su.tsw = AIR;
            if (curr == WATER) fillVerts(transparentverts, m_transparentvertcount, x, YMAX, z, curr, su);
            else if (curr != AIR) fillVerts(opaqueverts, m_opaquevertcount, x, YMAX, z, curr, su);
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
