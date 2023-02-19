#include "glad/glad.h"
#include "rendering/shader.hpp"

static const char *shaderTypeToString(u32 type)
{
    return type == GL_VERTEX_SHADER   ? "vertex"   :
           type == GL_FRAGMENT_SHADER ? "fragment" :
           "unknown";
}

static u32 compileShader(u32 type, const char *path)
{
    size_t size;
    char *src = (char *)readEntireFile(path, &size);
    if (!src)
        die("failed to open file %s", path);
    const char *sType = shaderTypeToString(type);
    GLuint shader = glCreateShader(type);
    if (!shader) {
        free(src);
        die("failed to create %s shader", sType);
    }
    i32 success = 0;
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        free(src);
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        die("while compiling %s shader:\n%s", sType, log);
    }
    free(src);
    return shader;
}

Shader::Shader(const char *vpath, const char *fpath)
{
    u32 vsh = compileShader(GL_VERTEX_SHADER, vpath);
    u32 fsh = compileShader(GL_FRAGMENT_SHADER, fpath);

    m_program = glCreateProgram();
    if (!m_program)
        die("failed to create program");

    glAttachShader(m_program, vsh);
    glAttachShader(m_program, fsh);

    GLint success = 0;
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(m_program, 512, nullptr, log);
        die("while linking:\n %s", log);
    }

    glDetachShader(m_program, vsh);
    glDetachShader(m_program, fsh);
    glDeleteShader(vsh);
    glDeleteShader(fsh);
}

void Shader::destroy()
{
    glDeleteProgram(m_program);
}

void Shader::bind() const
{
    glUseProgram(m_program);
}

void Shader::uniform(const std::string_view &name, Mat4 &mat) const
{
    i32 u = _uniformLocation(name);
    ASSERT(u != -1, "invalid uniform name");
    glUniformMatrix4fv(u, 1, GL_TRUE, &mat[0][0]);
}

void Shader::uniform(const std::string_view &name, f32 a, f32 b) const
{
    i32 u = _uniformLocation(name);
    ASSERT(u != -1, "invalid uniform name");
    glUniform2f(u, a, b);
}

void Shader::uniform(const std::string_view &name, Vec3 &v3) const
{
    i32 u = _uniformLocation(name);
    ASSERT(u != -1, "invalid uniform name");
    glUniform3f(u, v3.x, v3.y, v3.z);
}

void Shader::uniform(const std::string_view &name, i32 i) const
{
    i32 u = _uniformLocation(name);
    ASSERT(u != -1, "invalid uniform name");
    glUniform1i(u, i);
}

i32 Shader::_uniformLocation(const std::string_view &name) const
{
    i32 r;
    auto f = m_uniforms.find(name);
    (f == m_uniforms.end()) ?
        r = glGetUniformLocation(m_program, name.data()),
        m_uniforms[name] = r :
        r = f->second;
    return r;
}
