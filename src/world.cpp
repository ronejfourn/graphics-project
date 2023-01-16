#include "world.hpp"
#include "glad/glad.h"
#include <stdlib.h>

static i32 mod(i32 o, i32 n)
{
    if (o < 0) { o = n - ((-o) & (n - 1)); }
    return o & (n - 1);
}

World::World(const char *vsh, const char *fsh, u8 nchunks, Vec4 pos) :
    m_shader(vsh, fsh)
{
    m_xpos = (i32)floorf(pos.x / CHUNK_MAX_X);
    m_zpos = (i32)floorf(pos.z / CHUNK_MAX_Z);
    m_xoff = m_xpos - nchunks / 2;
    m_zoff = m_zpos - nchunks / 2;
    m_nchunks = nchunks;
    m_chunks  = (Chunk*)calloc(nchunks * nchunks, sizeof(Chunk));
    if (!m_chunks)
        die("failed to allocate chunks");
    u32 m = nchunks * nchunks;
    for (u32 i = 0; i < m; i++)
        m_chunks[i].init(m_shader);
    m_uvp = m_shader.uniform("viewproj");
}

void World::_loadNewChunks(
        i32 xmax, i32 xmin,
        i32 zmax, i32 zmin,
        i32 xinc, i32 zinc)
{
    for (i32 x = xmin; x <= xmax; x ++) {
        i32 xi = mod(x, m_nchunks);
        i32 bi = xi * m_nchunks;
        for (i32 z = zmin; z <= zmax; z ++) {
            i32 zi = mod(z, m_nchunks);
            Chunk &self = m_chunks[bi + zi];
            self.generate(x, z, m_fbmc);

            i32 xn, zn;
            self.setEast (nullptr);
            self.setWest (nullptr);
            self.setNorth(nullptr);
            self.setSouth(nullptr);

            if (x > xmin || xinc > 0) {
                xn = mod(x - 1, m_nchunks);
                zn = zi;
                Chunk *w = &m_chunks[xn * m_nchunks + zn];
                w->setEast(&self);
                self.setWest(w);
            }

            if (x < xmax || xinc < 0) {
                xn = mod(x + 1, m_nchunks);
                zn = zi;
                Chunk *e = &m_chunks[xn * m_nchunks + zn];
                e->setWest(&self);
                self.setEast(e);
            }

            if (z > zmin || zinc > 0) {
                xn = xi;
                zn = mod(z - 1, m_nchunks);
                Chunk *s = &m_chunks[xn * m_nchunks + zn];
                s->setNorth(&self);
                self.setSouth(s);
            }

            if (z < zmax || zinc < 0) {
                xn = xi;
                zn = mod(z + 1, m_nchunks);
                Chunk *n = &m_chunks[xn * m_nchunks + zn];
                n->setSouth(&self);
                self.setNorth(n);
            }
        }
    }
}

void World::generate(u64 seed)
{
    m_fbmc = FBMConfig(seed);
    m_xoff = m_xpos - m_nchunks / 2;
    m_zoff = m_zpos - m_nchunks / 2;

    i32 xmin = m_xpos - m_nchunks / 2;
    i32 xmax = m_xpos + m_nchunks / 2 - 1;

    i32 zmin = m_zpos - m_nchunks / 2;
    i32 zmax = m_zpos + m_nchunks / 2 - 1;

    _loadNewChunks(xmax, xmin, zmax, zmin, 0, 0);
}

World::~World()
{
    if (m_chunks) {
        for (u32 i = 0; i < m_nchunks * m_nchunks; i++)
            m_chunks[i].destroy();
        free(m_chunks);
    }
}

void World::update(const Vec4 &pos)
{
    i32 nxpos = (i32)floorf(pos.x / CHUNK_MAX_X);
    i32 nzpos = (i32)floorf(pos.z / CHUNK_MAX_Z);

    if (nxpos == m_xpos && nzpos == m_zpos)
        return;

    i32 xmin, xmax, zmin, zmax;
    i32 xinc = nxpos - m_xpos, zinc = nzpos - m_zpos;
    m_xpos = nxpos, m_zpos = nzpos;

    xmin = 0, xmax = -1, zmin = 0, zmax = -1;

    if (zinc < 0) {
        zmin = m_zoff + zinc;
        zmax = m_zoff - 1;
        xmin = m_xoff, xmax = m_xoff + m_nchunks - 1;
    } else if (zinc > 0) {
        zmin = m_zoff + m_nchunks;
        zmax = m_zoff + m_nchunks + zinc - 1;
        xmin = m_xoff, xmax = m_xoff + m_nchunks - 1;
    }

    _loadNewChunks(xmax, xmin, zmax, zmin, 0, zinc);
    m_zoff += zinc;

    xmin = 0, xmax = -1, zmin = 0, zmax = -1;

    if (xinc < 0) {
        xmin = m_xoff + xinc;
        xmax = m_xoff - 1;
        zmin = m_zoff, zmax = m_zoff + m_nchunks - 1;
    } else if (xinc > 0) {
        xmin = m_xoff + m_nchunks;
        xmax = m_xoff + m_nchunks + xinc - 1;
        zmin = m_zoff, zmax = m_zoff + m_nchunks - 1;
    }

    _loadNewChunks(xmax, xmin, zmax, zmin, xinc, 0);
    m_xoff += xinc;
}

void World::render(Camera &cam) {
    m_shader.bind();
    Mat4 vp = cam.getProjectionMatrix() * cam.getViewMatrix();
    glUniformMatrix4fv(m_uvp, 1, GL_TRUE, &vp[0][0]);

    const i32 m = m_nchunks * m_nchunks;
    for (i32 i = 0; i < m; i ++)
        m_chunks[i].render();
}
