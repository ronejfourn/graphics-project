#pragma once

#include "chunk.hpp"
#include "common.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "noise.hpp"

class World {
public:
    World(const char *vsh,
          const char *fsh,
          u8 nchunks = 8,
          Vec4 pos = Vec4());
    ~World();

    void generate(u64 seed);
    void update(const Vec4 &pos);
    void render(Camera &camera);
private:
    Shader m_shader;
    u32 m_uvp;
    i32 m_xpos, m_zpos;
    i32 m_xoff, m_zoff;
    Chunk *m_chunks;
    u32 m_nchunks;
    FBMConfig m_fbmc;

    void _loadNewChunks(i32 xmax, i32 xmin,
                        i32 zmax, i32 zmin,
                        i32 xinc, i32 zinc);
};
