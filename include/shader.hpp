#pragma once

#include <glad/glad.h>

GLuint compile_shader(GLenum type, const char *src);
void   link_program(GLuint prog);
