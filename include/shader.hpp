#pragma once

#include "common.hpp"

class Shader
{
public:
    Shader(const char *vpath, const char *fpath);
    void bind();
    void destroy();
    u32 uniform(const char *name);
private:
    u32 m_program;
};
