#pragma once

#include "utility/common.hpp"

enum VertexUsage {
    DYNAMIC,
    STATIC,
};

enum VertexAttribType {
    FLOAT,
    UINT,
};

struct VertexAttrib {
    u32 location;
    u32 count;
    VertexAttribType type;
};

class VertexArray {
public:
     VertexArray(VertexUsage usage);
     VertexArray() {}
    ~VertexArray();
    void setData(u32 size, void *data);
    void subData(u32 size, void *data, u32 offset);
    void setAttribs(u32 nattribs, VertexAttrib *attribs);
    void bind();
private:
    u32 m_vao, m_vbo;
    u32 m_usage, m_vbsize;
};
