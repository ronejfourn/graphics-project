#pragma once

#define PROJECT_TITLE "Block Game"
#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3
#define DEFAULT_WINDOW_WIDTH  720
#define DEFAULT_WINDOW_HEIGHT 720

#include <stdarg.h>
#include <stdint.h>
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

#ifdef DEBUG
#define ASSERT(_cond, _msg) assert((_cond) && (_msg))
#else
#define ASSERT(_cond, _msg)
#endif
