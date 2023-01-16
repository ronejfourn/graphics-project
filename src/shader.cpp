#include "shader.hpp"
#include "common.hpp"

static const char *shader_type_to_string(GLenum type)
{
    return type == GL_VERTEX_SHADER   ? "vertex"   :
           type == GL_FRAGMENT_SHADER ? "fragment" :
           type == GL_GEOMETRY_SHADER ? "geometry" :
           "unknown";
}

GLuint compile_shader(GLenum type, const char *src)
{
    GLint  su = 0;
    GLuint sh = glCreateShader(type);
    if (!sh) {
        die("failed to create %s shader",
            shader_type_to_string(type));
    }
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);
    glGetShaderiv(sh, GL_COMPILE_STATUS, &su);
    if (!su) {
        GLchar log[1024];
        glGetShaderInfoLog(sh, 1024, nullptr, log);
        die("failed to compile %s shader:\n%s",
            shader_type_to_string(type), log);
    }
    return sh;
}

void link_program(GLuint prog)
{
    GLint su = 0;
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &su);
    if (!su) {
        GLchar log[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, log);
        die("failed to link program:\n%s", log);
    }
}
