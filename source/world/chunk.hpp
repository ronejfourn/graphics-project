#pragma once

#include "block.hpp"
#include "math/vector.hpp"
#include "utility/common.hpp"
#include "rendering/vertexArray.hpp"

enum ChunkState {
    Initial,
    NeedsUpdating,
    Ready,
};

constexpr u32 CHUNK_MAX_Y = 255;
constexpr u32 CHUNK_MAX_X = 15;
constexpr u32 CHUNK_MAX_Z = 15;

class Shader;
struct FBMConfig;
struct pcg32_random_t;

class Chunk {
public:
     Chunk();
    ~Chunk() = default;

    static Chunk *dummy() {
        static Chunk dummy(AIR);
        return &dummy;
    }

    void generate(i32 x, i32 z, FBMConfig &fc);
    void update();
    void renderPrep(const Shader &shader);
    void renderOpaque(const Shader &shader);
    void renderTransparent(const Shader &shader);

    void setEast (Chunk *p);
    void setWest (Chunk *p);
    void setNorth(Chunk *p);
    void setSouth(Chunk *p);
    void setNorthEast(Chunk *p);
    void setSouthEast(Chunk *p);
    void setNorthWest(Chunk *p);
    void setSouthWest(Chunk *p);

    inline ChunkState getState() { return m_state; }
    inline Chunk *getEast () { return m_east; }
    inline Chunk *getWest () { return m_west; }
    inline Chunk *getNorth() { return m_north; }
    inline Chunk *getSouth() { return m_south; }
    inline Chunk *getNorthEast() { return m_northeast; }
    inline Chunk *getSouthEast() { return m_southeast; }
    inline Chunk *getNorthWest() { return m_northwest; }
    inline Chunk *getSouthWest() { return m_southwest; }
    inline Vec3  getCenter() { return {m_origin.x + CHUNK_MAX_X / 2.0f, 0, m_origin.z + CHUNK_MAX_Z / 2.0f}; }

private:
    Chunk(u8 t);

    Chunk *m_east, *m_west, *m_north, *m_south;
    Chunk *m_southeast, *m_southwest;
    Chunk *m_northeast, *m_northwest;

    ChunkState m_state;
    Vec3 m_renderOrigin, m_origin;
    u8 m_blocks[CHUNK_MAX_X][CHUNK_MAX_Z][CHUNK_MAX_Y];
    VertexArray m_vao;
    u32 m_opaquevertcount;
    u32 m_transparentvertcount;

    /// <summary>
    /// Draws the main body of the tree
    /// </summary>
    void _placeOakTree(i32 x, i32 y, i32 z, pcg32_random_t *seed);

    /// <summary>
    /// Makes sure tree spawns near the centre so leaves don't get cut off
    /// </summary>
    bool _checkForOakTree(i32 x, i32 y, i32 z);
};
