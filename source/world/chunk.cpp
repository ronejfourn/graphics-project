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
    memset(m_blocks, 0, sizeof(m_blocks));
    m_opaquevertcount = 0;
    m_transparentvertcount = 0;
    m_changed = false;

    m_vao.bind();
    VertexAttrib va = {0, 1, UINT};
    m_vao.setAttribs(1, &va);
}

void Chunk::setEast(Chunk *p)
{
    m_east = p;
    m_changed = true;
}

void Chunk::setWest(Chunk *p)
{
    m_west = p;
    m_changed = true;
}

void Chunk::setNorth(Chunk *p)
{
    m_north = p;
    m_changed = true;
}

void Chunk::setSouth(Chunk *p)
{
    m_south = p;
    m_changed = true;
}

bool Chunk::_checkForOakTree(i32 x, i32 y, i32 z)
{
    if (x < 9 && x > 6 &&
        z < 9 && z > 6 &&
        y > seaLevel &&
        m_blocks[y][x][z] == GRASS)
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
        if (m_blocks[y][x][z] == AIR)
            m_blocks[y][x][z] = OAKLEAF;
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
        m_blocks[k][x][z] = OAKTREETRUNK;
        if((count ++) < max) {
            u32 radius = count / 2 + 2;
            placeLeaves(x, k, z, radius);
        }
    }

    m_blocks[y - 1][x][z] = DIRT;
    m_blocks[y + treeHeight][x][z] = OAKLEAF;
}

void Chunk::generate(i32 x, i32 z, FBMConfig& fc)
{
    pcg32_random_t rng = { (((u64)x * 3452189327901ull + 12682897369ull) * ((u64)z * 129728736478123ull + 1987724839021ull)) | 1, 32874012398623949ull };

    m_changed = true;
    memset(m_blocks, 0, sizeof(m_blocks));
    m_origin = Vec3(x, 0, z);
    bool hasTree = false;

    for (u8 cx = 0; cx < CHUNK_MAX_X; cx++) {
        for (u8 cz = 0; cz < CHUNK_MAX_Z; cz++)  {
            f32 n = noise((x + cx / (f32)CHUNK_MAX_X) / 32.0f, (z + cz / (f32)CHUNK_MAX_Z) / 32.0f, fc.permutation) * 0.5 + 0.5;
            u8 height = (u8)(fbm(x + cx / (f32)CHUNK_MAX_X, z + cz / (f32)CHUNK_MAX_Z, fc)
                * maxHeight * n + baseHeight);

            if (height > seaLevel) {
                m_blocks[height - 1][cx][cz] = GRASS;
                for (i32 y = 0; y < height - 1; y++)
                    m_blocks[y][cx][cz] = DIRT;

                for (i32 y = seaLevel - 5; y <= seaLevel + 5; y++) {
                    i32 d = y - seaLevel + 1;
                    d = d < 0 ? -d : d;
                    auto &cur = m_blocks[y][cx][cz];
                    if ((cur == GRASS || cur == DIRT) && ((pcg32_random_r(&rng) & ((1 << d) - 1)) <= d))
                        cur = SAND;
                }

                if (!hasTree && _checkForOakTree(cx, height - 1, cz)) {
                    if (!(pcg32_random_r(&rng) & 15)) {
                        hasTree = true;
                        _placeOakTree(cx, height, cz, &rng);
                    }
                }
            } else {
                for (i32 y = height; y < seaLevel; y++)
                    m_blocks[y][cx][cz] = WATER;
                for (i32 y = 0; y < height; y++)
                    m_blocks[y][cx][cz] = SAND;
            }
        }
    }
}

Vec3 Chunk::getCenter()
{
    Vec3 o = {
        m_origin.x * CHUNK_MAX_X + CHUNK_MAX_X / 2.0f,
        0,
        m_origin.z * CHUNK_MAX_Z + CHUNK_MAX_Z / 2.0f,
    };
    return o;
}

void Chunk::render(Shader &shader)
{
    if (m_changed) _update();

    shader.bind();
    shader.uniform("xz", m_origin.x, m_origin.z);

    m_vao.bind();
    if (m_opaquevertcount)
        glDrawArrays(GL_TRIANGLES, 0, m_opaquevertcount);
    if (m_transparentvertcount) {
        glEnable(GL_BLEND);
        glDrawArrays(GL_TRIANGLES, m_opaquevertcount, m_transparentvertcount);
        glDisable(GL_BLEND);
    }
}

static constexpr u32 maxVertCount = CHUNK_MAX_X * CHUNK_MAX_Y * CHUNK_MAX_Z * 6 * 6;
static u32 opaqueverts[maxVertCount];
static u32 transparentverts[maxVertCount];

void Chunk::_update()
{
    m_changed = false;
    m_opaquevertcount = 0;
    m_transparentvertcount = 0;
    u8 eb, wb, tb, bb, nb, sb;

    for (u16 y = 0; y < CHUNK_MAX_Y; y ++) {
        for (u16 x = 0; x < CHUNK_MAX_X; x ++) {
            for (u16 z = 0; z < CHUNK_MAX_Z; z ++) {
                u8 type = m_blocks[y][x][z];
                if (type == AIR) continue;

                bb = (y > 0) ? m_blocks[y - 1][x][z] : AIR;
                tb = (y < CHUNK_MAX_Y - 1) ? m_blocks[y + 1][x][z] : AIR;

                sb = (z > 0) ? m_blocks[y][x][z - 1] : m_south ? m_south->m_blocks[y][x][CHUNK_MAX_Z - 1] : GRASS;
                nb = (z < CHUNK_MAX_Z - 1) ? m_blocks[y][x][z + 1] : m_north ? m_north->m_blocks[y][x][0] : GRASS;

                wb = (x > 0) ? m_blocks[y][x - 1][z] : m_west ? m_west->m_blocks[y][CHUNK_MAX_X - 1][z] : GRASS;
                eb = (x < CHUNK_MAX_X - 1) ? m_blocks[y][x + 1][z] : m_east ? m_east->m_blocks[y][0][z] : GRASS;

                (type != WATER) ?
                    fillVerts(opaqueverts, m_opaquevertcount, x, y, z, type, sb, nb, eb, wb, tb, bb) :
                    fillVerts(transparentverts, m_transparentvertcount, x, y, z, type, sb, nb, eb, wb, tb, bb);
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
