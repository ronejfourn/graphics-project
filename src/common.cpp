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

u8 *readEntireFile(const char *fileName, size_t *size)
{
    *size = 0;
    u8 * bf = nullptr;
    FILE *fp = fopen(fileName, "rb");
    if (!fp) { return nullptr; }
    fseek(fp, 0, SEEK_END);
    i32 sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz == -1) {
        fclose(fp);
        return nullptr;
    }
    bf = (u8*)calloc(sz + 1, 1);
    if (!bf) {
        fclose(fp);
        return nullptr;
    }
    fread(bf, 1, sz, fp);
    *size = sz;
    return bf;
}
