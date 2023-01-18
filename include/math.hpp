#pragma once

#include <math.h>
#include "common.hpp"

#define PI 3.1415926535898
#define DEG2RAD(_a) ((_a) * PI / 180.0)

// do we need templates?
union vec4;
union mat4;

union vec4
{
    struct {float x, y, z, w;};
    struct {float r, g, b, a;};
    float v[4];

    vec4() : v{0, 0, 0, 1} {}
    vec4(float a) : v{a, a, a, a} {}
    vec4(float x, float y, float z, float w = 1) : v{x, y, z, w} {}

    float &operator[] (int i)
    {
        ASSERT(i < 4, "index out of range");
        return v[i];
    }

    float operator[] (int i) const // C++ is beautiful
    {
        ASSERT(i < 4, "index out of range");
        return v[i];
    }
};

float dot (const vec4& a, const vec4& b);
vec4 cross(const vec4& a, const vec4& b);
vec4 operator+ (const vec4& a, const vec4& b);
vec4 operator- (const vec4& a, const vec4& b);
vec4 operator* (const vec4& a, const vec4& b);
vec4 operator/ (const vec4& a, const vec4& b);
vec4 operator* (const vec4& a, const float b);
vec4 operator* (const float b, const vec4& a);
vec4 operator/ (const vec4& a, const float b);
vec4 operator* (const vec4& a, const mat4& b);
vec4& operator+= (vec4& a, const vec4& b);
vec4& operator-= (vec4& a, const vec4& b);
vec4& operator*= (vec4& a, const vec4& b);
vec4& operator/= (vec4& a, const vec4& b);
vec4& operator*= (vec4& a, const float b);
vec4& operator/= (vec4& a, const float b);
vec4& operator*= (vec4& a, const mat4& b);

union mat4
{
    vec4 r[4];
    f32 m[4][4];

    mat4() {
        r[0] = {1, 0, 0, 0};
        r[1] = {0, 1, 0, 0};
        r[2] = {0, 0, 1, 0};
        r[3] = {0, 0, 0, 1};
    }

    mat4(float v) {
        r[0] = {v, 0, 0, 0};
        r[1] = {0, v, 0, 0};
        r[2] = {0, 0, v, 0};
        r[3] = {0, 0, 0, v};
    }

    mat4(vec4 r1, vec4 r2, vec4 r3, vec4 r4) : r{r1, r2, r3, r4} {};

    vec4 &operator[] (int i)
    {
        ASSERT(i < 4, "index out of range");
        return r[i];
    }

    const vec4 &operator[] (int i) const
    {
        ASSERT(i < 4, "index out of range");
        return r[i];
    }
};

mat4 mat4_identity();
mat4 mat4_translation(float x, float y, float z);
mat4 mat4_perspective(float znear, float zfar, float fov, float aspect_ratio);
mat4 mat4_rotation_x(float angle);
mat4 mat4_rotation_y(float angle);
mat4 mat4_rotation_z(float angle);
mat4 mat4_rotation_x(float cos_of_angle, float sin_of_angle);
mat4 mat4_rotation_y(float cos_of_angle, float sin_of_angle);
mat4 mat4_rotation_z(float cos_of_angle, float sin_of_angle);

mat4 operator+ (const mat4& a, const mat4& b);
mat4 operator- (const mat4& a, const mat4& b);
mat4 operator* (const mat4& a, const mat4& b);
vec4 operator* (const mat4& a, const vec4& b);
mat4 operator* (const mat4& a, const float b);
mat4 operator* (const float b, const mat4& a);
mat4 operator/ (const mat4& a, const float b);

mat4& operator+= (mat4& a, const mat4& b);
mat4& operator-= (mat4& a, const mat4& b);
mat4& operator*= (mat4& a, const mat4& b);
vec4& operator*= (mat4& a, const vec4& b);
mat4& operator*= (mat4& a, const float b);
mat4& operator/= (mat4& a, const float b);
