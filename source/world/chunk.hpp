#pragma once

#include "math/vector.hpp"
#include "utility/common.hpp"
#include "rendering/vertexArray.hpp"

constexpr u32 CHUNK_MAX_Y = 256;
constexpr u32 CHUNK_MAX_X = 16;
constexpr u32 CHUNK_MAX_Z = 16;

class Shader;
struct FBMConfig;

class Chunk {
public:
     Chunk();
    ~Chunk() = default;

    Vec3 getOrigin();
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
    u32 m_vertcount;

    void _update();
};
