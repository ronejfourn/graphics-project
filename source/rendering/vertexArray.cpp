#include "glad/glad.h"
#include "vertexArray.hpp"

VertexArray::VertexArray(VertexUsage usage)
{
    ASSERT(usage == DYNAMIC || usage == STATIC, "unknown vertex usage");

    m_usage = usage == DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    m_vbsize = 0;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
}

VertexArray::~VertexArray()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void VertexArray::setData(u32 size, void *data)
{
    if (size > m_vbsize) {
        glBufferData(GL_ARRAY_BUFFER, size, data, m_usage);
        m_vbsize = size;
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }
}

void VertexArray::subData(u32 size, void *data, u32 offset)
{
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VertexArray::setAttribs(u32 nattribs, VertexAttrib *attribs)
{
    ASSERT(nattribs && attribs, "invalid attributes");
    u32 stride = 0, size;
    for (u32 i = 0; i < nattribs; i ++) {
        u32 t = attribs[i].type;
        u32 c = attribs[i].count;
        switch (t) {
            case FLOAT: size = c * sizeof(f32); break;
            case UINT : size = c * sizeof(u32); break;
            default   : ASSERT(0, "unknown attrib type"); break;
        }
        stride += size;
    }
    u8 *off = 0;
    for (u32 i = 0; i < nattribs; i ++) {
        u32 l = attribs[i].location;
        u32 t = attribs[i].type;
        u32 c = attribs[i].count;
        switch (t) {
            case FLOAT:
                glVertexAttribPointer(l, c, GL_FLOAT, GL_FALSE, stride, (void *)off);
                size = c * sizeof(f32);
                break;
            case UINT:
                glVertexAttribIPointer(l, c, GL_UNSIGNED_INT, stride, (void *)off);
                size = c * sizeof(u32);
                break;
        }
        off += size;
        glEnableVertexAttribArray(l);
    }
}

void VertexArray::bind() {
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}
