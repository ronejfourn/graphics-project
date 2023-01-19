#pragma once

#include <math.h>
#include "common.hpp"

#define PI 3.1415926535898
#define DEG2RAD(_a) ((_a) * PI / 180.0)

// do we need templates?
union Vec4;
union Mat4;

union Vec4
{
    struct {f32 x, y, z, w;};
    struct {f32 r, g, b, a;};
    f32 v[4];

    Vec4() : v{0, 0, 0, 1} {}
    Vec4(f32 a) : v{a, a, a, a} {}
    Vec4(f32 x, f32 y, f32 z, f32 w = 1) : v{x, y, z, w} {}

    f32 &operator[] (i32 i)
    {
        ASSERT(i >= 0 && i < 4, "index out of range");
        return v[i];
    }

    f32 operator[] (i32 i) const // C++ is beautiful
    {
        ASSERT(i >= 0 && i < 4, "index out of range");
        return v[i];
    }
};

f32  dot  (const Vec4& a, const Vec4& b);
Vec4 cross(const Vec4& a, const Vec4& b);
Vec4 operator+ (const Vec4& a, const Vec4& b);
Vec4 operator- (const Vec4& a, const Vec4& b);
Vec4 operator* (const Vec4& a, const Vec4& b);
Vec4 operator/ (const Vec4& a, const Vec4& b);
Vec4 operator* (const Vec4& a, const f32 b);
Vec4 operator* (const f32 b, const Vec4& a);
Vec4 operator/ (const Vec4& a, const f32 b);
Vec4 operator* (const Vec4& a, const Mat4& b);
Vec4& operator+= (Vec4& a, const Vec4& b);
Vec4& operator-= (Vec4& a, const Vec4& b);
Vec4& operator*= (Vec4& a, const Vec4& b);
Vec4& operator/= (Vec4& a, const Vec4& b);
Vec4& operator*= (Vec4& a, const f32 b);
Vec4& operator/= (Vec4& a, const f32 b);
Vec4& operator*= (Vec4& a, const Mat4& b);

union Mat4
{
    Vec4 r[4];
    f32 m[4][4];

    Mat4() {
        r[0] = {1, 0, 0, 0};
        r[1] = {0, 1, 0, 0};
        r[2] = {0, 0, 1, 0};
        r[3] = {0, 0, 0, 1};
    }

    Mat4(f32 v) {
        r[0] = {v, 0, 0, 0};
        r[1] = {0, v, 0, 0};
        r[2] = {0, 0, v, 0};
        r[3] = {0, 0, 0, v};
    }

    Mat4(Vec4 r1, Vec4 r2, Vec4 r3, Vec4 r4) : r{r1, r2, r3, r4} {};

    Vec4 &operator[] (int i)
    {
        ASSERT(i < 4, "index out of range");
        return r[i];
    }

    const Vec4 &operator[] (int i) const
    {
        ASSERT(i < 4, "index out of range");
        return r[i];
    }
};

Mat4 mat4Identity();
Mat4 mat4Translation(f32 x, f32 y, f32 z);
Mat4 mat4Perspective(f32 znear, f32 zfar, f32 fov, f32 aspectRatio);
Mat4 mat4RotationX(f32 angle);
Mat4 mat4RotationY(f32 angle);
Mat4 mat4RotationZ(f32 angle);
Mat4 mat4RotationX(f32 cosOfAngle, f32 sinOfAngle);
Mat4 mat4RotationY(f32 cosOfAngle, f32 sinOfAngle);
Mat4 mat4RotationZ(f32 cosOfAngle, f32 sinOfAngle);

Mat4 operator+ (const Mat4& a, const Mat4& b);
Mat4 operator- (const Mat4& a, const Mat4& b);
Mat4 operator* (const Mat4& a, const Mat4& b);
Vec4 operator* (const Mat4& a, const Vec4& b);
Mat4 operator* (const Mat4& a, const f32 b);
Mat4 operator* (const f32 b, const Mat4& a);
Mat4 operator/ (const Mat4& a, const f32 b);

Mat4& operator+= (Mat4& a, const Mat4& b);
Mat4& operator-= (Mat4& a, const Mat4& b);
Mat4& operator*= (Mat4& a, const Mat4& b);
Vec4& operator*= (Mat4& a, const Vec4& b);
Mat4& operator*= (Mat4& a, const f32 b);
Mat4& operator/= (Mat4& a, const f32 b);
