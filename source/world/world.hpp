#pragma once

#include "world/chunk.hpp"
#include "utility/common.hpp"
#include "utility/noise.hpp"
#include "rendering/shader.hpp"
#include "rendering/camera.hpp"
#include "rendering/textureArray.hpp"

class World {
public:
    World(u8 nchunks = 8,
          Vec4 pos = Vec4());
    ~World();

    void generate(u64 seed);
    void update(const Vec3 &pos);
    void render(Camera &camera);
private:
    Shader m_shader;
    i32 m_xpos, m_zpos;
    i32 m_xoff, m_zoff;
    Chunk *m_chunks;
    u32 m_nchunks;
    FBMConfig m_fbmc;
    TextureArray m_textureArray;

    void _loadNewChunks(i32 xmax, i32 xmin,
                        i32 zmax, i32 zmin,
                        i32 xinc, i32 zinc);
};
