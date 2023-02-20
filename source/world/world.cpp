#include "math/matrix.hpp"
#include "world/world.hpp"
#include "world/chunk.hpp"
#include "world/block.hpp"
#include "rendering/shader.hpp"

struct ChunkDistPair {
    Chunk *ptr;
    f32 dist;
};

static i32 mod(i32 o, i32 n)
{
    if (o < 0)
        o = n - ((-o) & (n - 1));
    return o & (n - 1);
}

World::World(u32 nchunks) :
    m_textureArray(0, BLOCK_TEXTURE_FILE, BLOCK_TILES_PER_ROW, BLOCK_TILES_PER_COLUMN)
{
    m_nchunks = nchunks;
    m_chunks = new Chunk[nchunks * nchunks];
    if (!m_chunks)
        die("out of memory");

    m_sortedChunks = new ChunkDistPair[nchunks * nchunks];
    if (!m_sortedChunks)
        die("out of memory");
}

void World::_loadNewChunks(i32 xmax, i32 xmin, i32 zmax, i32 zmin, i32 xinc, i32 zinc)
{
    for (i32 x = xmin; x <= xmax; x++) {
        i32 xi = mod(x, m_nchunks);
        i32 bi = xi * m_nchunks;
        for (i32 z = zmin; z <= zmax; z++) {
            i32 zi = mod(z, m_nchunks);
            Chunk &self = m_chunks[bi + zi];

            self.getEast     ()->setWest     (Chunk::dummy());
            self.getWest     ()->setEast     (Chunk::dummy());
            self.getNorth    ()->setSouth    (Chunk::dummy());
            self.getSouth    ()->setNorth    (Chunk::dummy());
            self.getNorthEast()->setSouthWest(Chunk::dummy());
            self.getSouthEast()->setNorthWest(Chunk::dummy());
            self.getNorthWest()->setSouthEast(Chunk::dummy());
            self.getSouthWest()->setNorthEast(Chunk::dummy());

            self.setEast     (Chunk::dummy());
            self.setWest     (Chunk::dummy());
            self.setNorth    (Chunk::dummy());
            self.setSouth    (Chunk::dummy());
            self.setNorthEast(Chunk::dummy());
            self.setSouthEast(Chunk::dummy());
            self.setNorthWest(Chunk::dummy());
            self.setSouthWest(Chunk::dummy());
        }
    }

    i32 xn, zn;
    for (i32 x = xmin; x <= xmax; x++) {
        i32 xi = mod(x, m_nchunks);
        i32 bi = xi * m_nchunks;
        for (i32 z = zmin; z <= zmax; z++) {
            i32 zi = mod(z, m_nchunks);
            Chunk &self = m_chunks[bi + zi];
            self.generate(x, z, m_fbmc);

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

                if (x > xmin || xinc > 0) {
                    xn = mod(x - 1, m_nchunks);
                    Chunk *sw = &m_chunks[xn * m_nchunks + zn];
                    sw->setNorthEast(&self);
                    self.setSouthWest(sw);
                }

                if (x < xmax || xinc < 0) {
                    xn = mod(x + 1, m_nchunks);
                    Chunk *se = &m_chunks[xn * m_nchunks + zn];
                    se->setNorthWest(&self);
                    self.setSouthEast(se);
                }
            }

            if (z < zmax || zinc < 0) {
                xn = xi;
                zn = mod(z + 1, m_nchunks);
                Chunk *n = &m_chunks[xn * m_nchunks + zn];
                n->setSouth(&self);
                self.setNorth(n);

                if (x > xmin || xinc > 0) {
                    xn = mod(x - 1, m_nchunks);
                    Chunk *nw = &m_chunks[xn * m_nchunks + zn];
                    nw->setSouthEast(&self);
                    self.setNorthWest(nw);
                }

                if (x < xmax || xinc < 0) {
                    xn = mod(x + 1, m_nchunks);
                    Chunk *ne = &m_chunks[xn * m_nchunks + zn];
                    ne->setSouthWest(&self);
                    self.setNorthEast(ne);
                }
            }
        }
    }
}

void World::generate(u64 seed, const Vec3 &pos)
{
    m_xpos = (i32)floorf(pos.x / CHUNK_MAX_X);
    m_zpos = (i32)floorf(pos.z / CHUNK_MAX_Z);
    m_xoff = m_xpos - m_nchunks / 2;
    m_zoff = m_zpos - m_nchunks / 2;

    m_fbmc = FBMConfig(seed);
    m_xoff = m_xpos - m_nchunks / 2;
    m_zoff = m_zpos - m_nchunks / 2;

    i32 xmin = m_xpos - m_nchunks / 2;
    i32 xmax = m_xpos + m_nchunks / 2 - 1;

    i32 zmin = m_zpos - m_nchunks / 2;
    i32 zmax = m_zpos + m_nchunks / 2 - 1;

    _loadNewChunks(xmax, xmin, zmax, zmin, 0, 0);
    _sortChunks(pos);
}

void World::_sortChunks(const Vec3 &pos)
{
    const i32 m = m_nchunks * m_nchunks;
    for (i32 i = 0; i < m; i++) {
        Vec3 orig = m_chunks[i].getCenter();
        float dist = squareMagnitude(orig - pos);
        m_sortedChunks[i] = {&m_chunks[i], dist};
    }

    auto compare = [](const void *a, const void *b)->int {
        return ((ChunkDistPair *)a)->dist < ((ChunkDistPair *)b)->dist;
    };

    qsort(m_sortedChunks, m, sizeof(ChunkDistPair), compare);
}

World::~World()
{
    if (m_chunks)
        delete[] m_chunks;
}

void World::update(const Vec3 &pos)
{
    i32 nxpos = (i32)floorf(pos.x / CHUNK_MAX_X);
    i32 nzpos = (i32)floorf(pos.z / CHUNK_MAX_Z);

    if (nxpos != m_xpos) {
        i32 xmin, xmax, zmin, zmax;
        i32 xinc = nxpos - m_xpos;
        if (xinc < 0) {
            xmin = m_xoff + xinc;
            xmax = m_xoff - 1;
            zmin = m_zoff, zmax = m_zoff + m_nchunks - 1;
        } else {
            xmin = m_xoff + m_nchunks;
            xmax = m_xoff + m_nchunks + xinc - 1;
            zmin = m_zoff, zmax = m_zoff + m_nchunks - 1;
        }
        _loadNewChunks(xmax, xmin, zmax, zmin, xinc, 0);
        m_xoff += xinc;
        m_xpos = nxpos;
    }

    if (nzpos != m_zpos) {
        i32 xmin, xmax, zmin, zmax;
        i32 zinc = nzpos - m_zpos;
        m_xpos = nxpos, m_zpos = nzpos;
        if (zinc < 0) {
            zmin = m_zoff + zinc;
            zmax = m_zoff - 1;
            xmin = m_xoff, xmax = m_xoff + m_nchunks - 1;
        } else {
            zmin = m_zoff + m_nchunks;
            zmax = m_zoff + m_nchunks + zinc - 1;
            xmin = m_xoff, xmax = m_xoff + m_nchunks - 1;
        }
        _loadNewChunks(xmax, xmin, zmax, zmin, 0, zinc);
        m_zoff += zinc;
        m_zpos = nzpos;
    }

    _sortChunks(pos);

    u32 c = 0;
    const i32 m = m_nchunks * m_nchunks;
    for (i32 i = m - 1; i >= 0 && c < 8; i--) {
        if (m_sortedChunks[i].ptr->getState() == NeedsUpdating) {
            m_sortedChunks[i].ptr->update();
            c ++;
        }
    }
}

void World::depthPass(const Shader &shader)
{
    const i32 m = m_nchunks * m_nchunks;
    for (i32 i = 0; i < m; i++) {
        m_sortedChunks[i].ptr->renderPrep(shader);
        m_sortedChunks[i].ptr->renderOpaque(shader);
    }
}

void World::renderPass(const Shader &shader)
{
    m_textureArray.bind();
    const i32 m = m_nchunks * m_nchunks;
    for (i32 i = 0; i < m; i++) {
        m_sortedChunks[i].ptr->renderPrep(shader);
        m_sortedChunks[i].ptr->renderOpaque(shader);
        m_sortedChunks[i].ptr->renderTransparent(shader);
    }
}
