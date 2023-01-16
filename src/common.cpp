#include "common.hpp"
#include <stdio.h>
#include <stdlib.h>

void die(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fputc('\n', stderr);
    exit(1);
}
