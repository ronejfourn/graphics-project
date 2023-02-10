#pragma once

#include "math/vector.hpp"
#include "utility/common.hpp"
#include "rendering/vertexArray.hpp"

constexpr u32 CHUNK_MAX_Y = 256;
constexpr u32 CHUNK_MAX_X = 16;
constexpr u32 CHUNK_MAX_Z = 16;

class Shader;
struct FBMConfig;
struct pcg32_random_t;

class Chunk {
public:
     Chunk();
    ~Chunk() = default;

    Vec3 getCenter();
    void render(Shader &shader);
    void generate(i32 x, i32 z, FBMConfig &fc);
    void setEast (Chunk *p);
    void setWest (Chunk *p);
    void setNorth(Chunk *p);
    void setSouth(Chunk *p);

private:
    Chunk *m_east, *m_west, *m_north, *m_south;
    bool m_changed;
    Vec3 m_origin;
    u8 m_blocks[CHUNK_MAX_Y][CHUNK_MAX_X][CHUNK_MAX_Z];
    VertexArray m_vao;
    u32 m_opaquevertcount;
    u32 m_transparentvertcount;

    void _update();

    /// <summary>
    /// Draws the main body of the tree
    /// </summary>
    void _placeOakTree(i32 x, i32 y, i32 z, pcg32_random_t *seed);

    /// <summary>
    /// Makes sure tree spawns near the centre so leaves don't get cut off
    /// </summary>
    bool _checkForOakTree(i32 x, i32 y, i32 z);
};
