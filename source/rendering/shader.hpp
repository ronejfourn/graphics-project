#pragma once

#include "utility/common.hpp"
#include "math/matrix.hpp"
#include <unordered_map>
#include <string_view>

class Shader
{
public:
    Shader(const char *vpath, const char *fpath);
    void bind();
    void destroy();
    void uniform(const std::string_view &name, Mat4& mat);
    void uniform(const std::string_view &name, f32 a, f32 b);
private:
    u32 m_program;
    std::unordered_map<std::string_view, i32> m_uniforms;

    i32 _uniformLocation(const std::string_view &name);
};
