#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef int8_t   i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

#define BITSET(v, b) (v) | (1 << (b))
#define BITRESET(v, b) (v) & (~(1 << (b)))
#define BITTOGGLE(v, b) (v) ^ (1 << (b))

void die(const char *fmt, ...);
u8* readEntireFile(const char *fileName, size_t *size);

#ifdef DEBUG
#define ASSERT(_cond, _msg) assert((_cond) && (_msg))
#else
#define ASSERT(_cond, _msg)
#endif
