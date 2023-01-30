#pragma once

#include "vector.hpp"

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

inline Mat4 mat4Identity()
{
    return {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };
}

inline Mat4 mat4Translation(f32 x, f32 y, f32 z)
{
    return {
        {1, 0, 0, x},
        {0, 1, 0, y},
        {0, 0, 1, z},
        {0, 0, 0, 1},
    };
}

inline Mat4 mat4Perspective(f32 zn, f32 zf, f32 fov, f32 ar)
{
    f32 zr = zn - zf;
    f32 a = (float)DEG2RAD(fov / 2);
    f32 y = 1 / tanf(a);
    f32 x = y / ar;
    f32 p = - (zn + zf) / zr;
    f32 q = 2 * zn * zf / zr;
    return {
        {x, 0, 0, 0},
        {0, y, 0, 0},
        {0, 0, p, q},
        {0, 0, 1, 0},
    };
}

inline Mat4 mat4RotationX(f32 c, f32 s)
{
    return {
        {1, 0,  0, 0},
        {0, c, -s, 0},
        {0, s,  c, 0},
        {0, 0,  0, 1},
    };
}

inline Mat4 mat4RotationY(f32 c, f32 s)
{
    return {
        {c, 0, -s, 0},
        {0, 1,  0, 0},
        {s, 0,  c, 0},
        {0, 0,  0, 1},
    };
}

inline Mat4 mat4RotationZ(f32 c, f32 s)
{
    return {
        {c, -s, 0, 0},
        {s,  c, 0, 0},
        {0,  0, 1, 0},
        {0,  0, 0, 1},
    };
}

inline Mat4 mat4RotationX(f32 a) {return mat4RotationX(cosf(a), sinf(a));}
inline Mat4 mat4RotationY(f32 a) {return mat4RotationY(cosf(a), sinf(a));}
inline Mat4 mat4RotationZ(f32 a) {return mat4RotationZ(cosf(a), sinf(a));}

inline Mat4 mat4LookAt(Vec3 position, Vec3 front, Vec3 up){
    Vec3 f = normalize(front);
    Vec3 r = normalize(cross(up,f));
    Vec3 u = cross(f,r);
    Mat4 view={
        Vec4(r.x, r.y, r.z, -dot(r,position)),
        Vec4(u.x, u.y, u.z, -dot(u,position)),
        Vec4(f.x, f.y, f.z, -dot(f,position)),
        Vec4(  0,   0,   0,                1),
    };
    return view;
}

inline Mat4 operator+ (const Mat4& a, const Mat4& b) {return {a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]};}
inline Mat4 operator- (const Mat4& a, const Mat4& b) {return {a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]};}
inline Mat4 operator* (const Mat4& a, const f32 b) {return {a[0] * b, a[1] * b, a[2] * b, a[3] * b};}
inline Mat4 operator* (const f32 b, const Mat4& a) {return {a[0] * b, a[1] * b, a[2] * b, a[3] * b};}
inline Mat4 operator/ (const Mat4& a, const f32 b) {return {a[0] / b, a[1] / b, a[2] / b, a[3] / b};}

inline Vec4 operator* (const Vec4& a, const Mat4& b)
{
    Vec4 x, y, z, w;
    x = a.x * b[0];
    y = a.y * b[1];
    z = a.z * b[2];
    w = a.w * b[3];
    return x + y + z + w;
}

inline Vec4 operator* (const Mat4& a, const Vec4& b)
{
    return {
        dot(a[0], b),
        dot(a[1], b),
        dot(a[2], b),
        dot(a[3], b),
    };
}

inline Mat4 operator* (const Mat4& a, const Mat4& b)
{
    Mat4 c;
    c[0] = a[0] * b;
    c[1] = a[1] * b;
    c[2] = a[2] * b;
    c[3] = a[3] * b;
    return c;
}

inline Mat4& operator+= (Mat4& a, const Mat4& b) {a = a + b; return a;}
inline Mat4& operator-= (Mat4& a, const Mat4& b) {a = a - b; return a;}
inline Mat4& operator*= (Mat4& a, const Mat4& b) {a = a * b; return a;}
inline Mat4& operator*= (Mat4& a, const f32 b) {a = a * b; return a;}
inline Mat4& operator/= (Mat4& a, const f32 b) {a = a / b; return a;}
