#pragma once

#include <glad/glad.h>

GLuint compileShader(GLenum type, const char *src);
void   linkProgram(GLuint prog);
