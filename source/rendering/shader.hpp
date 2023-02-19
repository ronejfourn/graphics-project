#pragma once

#include "utility/common.hpp"
#include "math/matrix.hpp"
#include <unordered_map>
#include <string_view>

class Shader
{
public:
    Shader(const char *vpath, const char *fpath);
    void bind() const;
    void destroy();
    void uniform(const std::string_view &name, Mat4& mat) const;
    void uniform(const std::string_view &name, f32 a, f32 b) const;
    void uniform(const std::string_view &name, Vec3& v3) const;
    void uniform(const std::string_view &name, i32 i) const;
private:
    u32 m_program;
    mutable std::unordered_map<std::string_view, i32> m_uniforms;

    i32 _uniformLocation(const std::string_view &name) const;
};
