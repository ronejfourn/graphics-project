#pragma once

#include "common.hpp"

struct FBMConfig {
    f32 lacunarity;
    f32 gain;
    f32 scale;
    u32 octaves;
    u64 seed;
    u8  permutation[512];

    FBMConfig(u64 _seed       = 0,
              u32 _octaves    = 4,
              f32 _lacunarity = 2.0f,
              f32 _gain       = 0.5f,
              f32 _scale      = 16.0f);
};

f32 noise(f32 xin, f32 yin, u8 perm[512]);
f32 fbm(f32 x, f32 y, FBMConfig &fc);
