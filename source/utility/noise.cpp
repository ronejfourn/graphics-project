#include "noise.hpp"
#include <math.h>

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

static uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

FBMConfig::FBMConfig(u64 _seed, u32 _octaves, f32 _lacunarity, f32 _gain, f32 _scale) :
    seed(_seed), octaves(_octaves), lacunarity(_lacunarity), gain(_gain), scale(_scale)
{
    pcg32_random_t rng{_seed, 1};

    u8 p[256];
    for (u32 i = 0; i < 256; i ++)
        p[i] = i;
    for (u32 i = 255; i > 0; i --) {
        u32 j = pcg32_random_r(&rng) % i;
        u8 t = p[i];
        p[i] = p[j];
        p[j] = t;
    }
    for (u32 i = 0; i < 512; i ++)
        permutation[i] = p[i & 255];
}

f32 fbm(f32 x, f32 y, FBMConfig &fc)
{
    f32 xs = x / fc.scale;
    f32 ys = y / fc.scale;

    f32 normalize = 0.0f;
    f32 amplitude = 1.0f;
    f32 frequency = 1.0f;
    f32 total = 0.0f;

    for (u32 i = 0; i < fc.octaves; i ++) {
        total += amplitude * (noise(xs * frequency, ys * frequency, fc.permutation) * 0.5f + 0.5f);
        normalize += amplitude;
        amplitude *= fc.gain;
        frequency *= fc.lacunarity;
    }

    return total / normalize;
}

/*
 * https://weber.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
 */

static struct Grad { i32 x, y, z; } grad3[] = {
    {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
    {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
    {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}
};

static f32 dot(Grad g, f32 x, f32 y){
    return g.x * x + g.y * y;
}

// 2D simplex noise
f32 noise(f32 xin, f32 yin, u8 perm[512])
{
    f32 n0, n1, n2; // Noise contributions from the three corners
    // Skew the input space to determine which simplex cell we're in
    f32 F2 = 0.5f*(sqrtf(3.0f)-1.0f);
    f32 s = (xin+yin)*F2; // Hairy factor for 2D
    i32 i = (i32)floorf(xin+s);
    i32 j = (i32)floorf(yin+s);
    f32 G2 = (3.0f-sqrtf(3.0f))/6.0f;
    f32 t = (i+j)*G2;
    f32 X0 = i-t; // Unskew the cell origin back to (x,y) space
    f32 Y0 = j-t;
    f32 x0 = xin-X0; // The x,y distances from the cell origin
    f32 y0 = yin-Y0;
    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    i32 i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if(x0>y0) {i1=1; j1=0;} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else {i1=0; j1=1;} // upper triangle, YX order: (0,0)->(0,1)->(1,1)
    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6
    f32 x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    f32 y1 = y0 - j1 + G2;
    f32 x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
    f32 y2 = y0 - 1.0f + 2.0f * G2;
    // Work out the hashed gradient indices of the three simplex corners
    i32 ii = i & 255;
    i32 jj = j & 255;
    i32 gi0 = perm[ii+perm[jj]] % 12;
    i32 gi1 = perm[ii+i1+perm[jj+j1]] % 12;
    i32 gi2 = perm[ii+1+perm[jj+1]] % 12;
    // Calculate the contribution from the three corners
    f32 t0 = 0.5f - x0*x0-y0*y0;
    if(t0<0) n0 = 0.0f;
    else {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
    }
    f32 t1 = 0.5f - x1*x1-y1*y1;
    if(t1<0) n1 = 0.0f;
    else {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
    }
    f32 t2 = 0.5f - x2*x2-y2*y2;
    if(t2<0) n2 = 0.0f;
    else {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return (70.0f * (n0 + n1 + n2));
}

