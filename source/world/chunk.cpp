#include "glad/glad.h"
#include "rendering/shader.hpp"
#include "world/chunk.hpp"
#include "world/block.hpp"
#include "utility/noise.hpp"

#include <math.h>
#include <memory.h>

static constexpr i32 seaLevel      = 65;
static constexpr  u8 baseHeight    = 45;
static constexpr  u8 maxHeight     = 80;

Chunk::Chunk() :
    m_vao(DYNAMIC)
{
    memset(m_blocks, 0, sizeof(m_blocks));
    m_vertcount = 0;
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

void Chunk::generate(i32 x, i32 z, FBMConfig &fc)
{
    m_changed = true;
    memset(m_blocks, 0, sizeof(m_blocks));
    m_origin = Vec3(x, 0, z);
    for (u8 cx = 0; cx < CHUNK_MAX_X; cx ++) {
        for (u8 cz = 0; cz < CHUNK_MAX_Z; cz ++) {
            u8 height = (u8)(fbm(x + cx / (f32)CHUNK_MAX_X, z + cz / (f32)CHUNK_MAX_Z, fc)
                * maxHeight + baseHeight);
            for (i32 y = 0; y < height; y ++)
                m_blocks[y][cx][cz] = GRASS;
            for (i32 y = height; y < seaLevel; y ++)
                m_blocks[y][cx][cz] = WATER;
        }
    }
}

Vec3 Chunk::getOrigin()
{
    return m_origin;
}

void Chunk::render(Shader &shader)
{
    if (m_changed) _update();

    shader.bind();
    shader.uniform("xz", m_origin.x, m_origin.z);

    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_vertcount);
}

static constexpr u32 maxVertCount = CHUNK_MAX_X * CHUNK_MAX_Y * CHUNK_MAX_Z * 6 * 6;
static u32 verts[maxVertCount];

void Chunk::_update()
{
    m_changed = false;

    u32 count = 0;
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

                fillVerts(verts, count, x, y, z, type, sb, nb, eb, wb, tb, bb);
            }
        }
    }

    m_vertcount = count;
    m_vao.bind();
    m_vao.setData(m_vertcount * 4, verts);
}
