#pragma once

#include "common.hpp"
#include "shader.hpp"

constexpr u32 CHUNK_MAX_Y = 256;
constexpr u32 CHUNK_MAX_X = 16;
constexpr u32 CHUNK_MAX_Z = 16;

class Shader;
struct FBMConfig;

class Chunk {
public:
     Chunk() = delete;
    ~Chunk() = delete;

    void init(Shader &shader);
    void destroy();
    void render();
    void generate(i32 x, i32 z, FBMConfig &fc);
    void setEast (Chunk *p);
    void setWest (Chunk *p);
    void setNorth(Chunk *p);
    void setSouth(Chunk *p);
private:
    Chunk *m_east, *m_west, *m_north, *m_south;
    Shader m_shader;
    bool m_changed;
    i32 m_x, m_z;
    u8 m_blocks[CHUNK_MAX_Y][CHUNK_MAX_X][CHUNK_MAX_Z];
    u32 m_vbo, m_vao, m_uxz;
    u32 m_vertmax, m_vertcount;

    void _update();
};
