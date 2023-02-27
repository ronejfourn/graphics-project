#pragma once

#include "math/vector.hpp"
#include "utility/common.hpp"
#include "utility/noise.hpp"
#include "rendering/textureArray.hpp"

class Shader;
class Chunk;
struct ChunkDistPair;
union Mat4;

class World {
public:
    World(u32 nchunks = 8);
    ~World();

    void generate(u64 seed, const Vec3 &pos);
    void update(const Vec3 &pos);
    void depthPass (const Shader &shader, const Mat4 &vp);
    void renderPass(const Shader &shader, const Mat4 &vp);
    const TextureArray &getTextureArray() { return m_textureArray; }
private:
    i32 m_xpos, m_zpos;
    i32 m_xoff, m_zoff;
    Chunk *m_chunks;
    ChunkDistPair *m_sortedChunks;
    u32 m_nchunks;
    FBMConfig m_fbmc;
    TextureArray m_textureArray;

    void _loadNewChunks(i32 xmax, i32 xmin, i32 zmax, i32 zmin, i32 xinc, i32 zinc);
    void _sortChunks(const Vec3 &pos);
};
