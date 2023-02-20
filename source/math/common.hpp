#pragma once

#include "utility/common.hpp"
#include <math.h>

#define PI 3.1415926535898
#define DEG2RAD(_a) ((_a) * PI / 180.0)

inline f32 min(f32 a, f32 b) { return a < b ? a : b; }
inline f32 max(f32 a, f32 b) { return a > b ? a : b; }
inline f32 lerp(f32 a, f32 b, f32 t) { return (1 - t) * a + t * b; }
