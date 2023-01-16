#include "chunk.hpp"
#include "glad/glad.h"
#include "shader.hpp"
#include "block.hpp"
#include "noise.hpp"

#include <math.h>
#include <memory.h>
#include <stdlib.h>

static constexpr i32 seaLevel      = 65;
static constexpr  u8 baseHeight    = 45;
static constexpr  u8 maxHeight     = 80;

void Chunk::init(Shader &shader)
{
    memset(m_blocks, 0, sizeof(m_blocks));
    m_vertcount = 0;
    m_vertmax = 0;
    m_changed = false;
    m_shader = shader;
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    m_uxz = shader.uniform("xz");

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);
    glEnableVertexAttribArray(0);
}

void Chunk::destroy()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
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
    m_x = x, m_z = z;
    for (u8 cx = 0; cx < CHUNK_MAX_X; cx ++) {
        for (u8 cz = 0; cz < CHUNK_MAX_Z; cz ++) {
            u8 height = (u8)(fbm(m_x + cx / (f32)CHUNK_MAX_X, m_z + cz / (f32)CHUNK_MAX_Z, fc)
                * maxHeight + baseHeight);
            for (i32 y = 0; y < height; y ++)
                m_blocks[y][cx][cz] = 1;
            for (i32 y = height; y < seaLevel; y ++)
                m_blocks[y][cx][cz] = 2;
        }
    }
}

void Chunk::render()
{
    if (m_changed) _update();

    m_shader.bind();
    glUniform2f(m_uxz, (f32)m_x, (f32)m_z);
    glBindVertexArray(m_vao);
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
                if (!type) continue;

                bb = (y > 0) ? m_blocks[y - 1][x][z] : 0;
                tb = (y < CHUNK_MAX_Y - 1) ? m_blocks[y + 1][x][z] : 0;

                sb = (z > 0) ? m_blocks[y][x][z - 1] : m_south ? m_south->m_blocks[y][x][CHUNK_MAX_Z - 1] : 0;
                nb = (z < CHUNK_MAX_Z - 1) ? m_blocks[y][x][z + 1] : m_north ? m_north->m_blocks[y][x][0] : 0;

                wb = (x > 0) ? m_blocks[y][x - 1][z] : m_west ? m_west->m_blocks[y][CHUNK_MAX_X - 1][z] : 0;
                eb = (x < CHUNK_MAX_X - 1) ? m_blocks[y][x + 1][z] : m_east ? m_east->m_blocks[y][0][z] : 0;

                fillVerts(verts, count, x, y, z, type, sb, nb, eb, wb, tb, bb);
            }
        }
    }

    m_vertcount = count;

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    if (m_vertcount > m_vertmax) {
        glBufferData(GL_ARRAY_BUFFER, m_vertcount * 4, verts, GL_DYNAMIC_DRAW);
        m_vertmax = m_vertcount;
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertcount * 4, verts);
    }
}
