#include "shader.hpp"
#include "common.hpp"
#include "glad/glad.h"
#include <cstdlib>
#include <stdio.h>

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
        die("while compiling %s shader:\n", log);
    }
    free(src);
    return shader;
}

Shader::Shader(const char *vpath, const char *fpath) {
    u32 vsh = compileShader(GL_VERTEX_SHADER  , vpath);
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

void Shader::destroy() {
    glDeleteProgram(m_program);
}

void Shader::bind() {
    glUseProgram(m_program);
}

u32 Shader::uniform(const char *name) {
    return glGetUniformLocation(m_program, name);
}
