#pragma once

#include "vector.hpp"

union Mat4
{
    Vec4 r[4];

    inline Mat4() {
        r[0] = {1, 0, 0, 0};
        r[1] = {0, 1, 0, 0};
        r[2] = {0, 0, 1, 0};
        r[3] = {0, 0, 0, 1};
    }

    inline Mat4(f32 v) {
        r[0] = {v, 0, 0, 0};
        r[1] = {0, v, 0, 0};
        r[2] = {0, 0, v, 0};
        r[3] = {0, 0, 0, v};
    }

    inline Mat4(const Vec4 &r1, const Vec4 &r2, const Vec4 &r3, const Vec4 &r4) :
        r{r1, r2, r3, r4} {};

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
    f32 a = (float)(fov / 2);
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

inline Mat4 mat4Orthographic(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    return {
        {2 / (r - l), 0, 0, -(r + l) / (r - l)},
        {0, 2 / (t - b), 0, -(t + b) / (t - b)},
        {0, 0, 2 / (f - n), -(f + n) / (f - n)},
        {0, 0, 0, 1}
    };
}

inline Mat4 mat4Orthographic(f32 r, f32 t, f32 f)
{
    // l = -r, b = -t, n = -f
    return {
        {1 / r, 0, 0, 0},
        {0, 1 / t, 0, 0},
        {0, 0, 1 / f, 0},
        {0, 0, 0, 1},
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

inline Mat4 mat4Inverse(const Mat4 &m) {
    Mat4 i;
    f32 a, b, c, d, e, f;

    a = (m[2][2] * m[3][3] - m[3][2] * m[2][3]);
    b = (m[2][1] * m[3][3] - m[3][1] * m[2][3]);
    c = (m[2][1] * m[3][2] - m[3][1] * m[2][2]);
    d = (m[2][0] * m[3][3] - m[3][0] * m[2][3]);
    e = (m[2][0] * m[3][2] - m[3][0] * m[2][2]);
    f = (m[2][0] * m[3][1] - m[3][0] * m[2][1]);

    i[0][0] = + m[1][1] * a - m[1][2] * b + m[1][3] * c;
    i[1][0] = - m[1][0] * a + m[1][2] * d - m[1][3] * e;
    i[2][0] = + m[1][0] * b - m[1][1] * d + m[1][3] * f;
    i[3][0] = - m[1][0] * c + m[1][1] * e - m[1][2] * f;

    i[0][1] = - m[0][1] * a + m[0][2] * b - m[0][3] * c;
    i[1][1] = + m[0][0] * a - m[0][2] * d + m[0][3] * e;
    i[2][1] = - m[0][0] * b + m[0][1] * d - m[0][3] * f;
    i[3][1] = + m[0][0] * c - m[0][1] * e + m[0][2] * f;

    a = (m[0][2] * m[1][3] - m[1][2] * m[0][3]);
    b = (m[0][1] * m[1][3] - m[1][1] * m[0][3]);
    c = (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
    d = (m[0][0] * m[1][3] - m[1][0] * m[0][3]);
    e = (m[0][0] * m[1][2] - m[1][0] * m[0][2]);
    f = (m[0][0] * m[1][1] - m[1][0] * m[0][1]);

    i[0][2] = + m[3][1] * a - m[3][2] * b + m[3][3] * c;
    i[1][2] = - m[3][0] * a + m[3][2] * d - m[3][3] * e;
    i[2][2] = + m[3][0] * b - m[3][1] * d + m[3][3] * f;
    i[3][2] = - m[3][0] * c + m[3][1] * e - m[3][2] * f;

    i[0][3] = - m[2][1] * a + m[2][2] * b - m[2][3] * c;
    i[1][3] = + m[2][0] * a - m[2][2] * d + m[2][3] * e;
    i[2][3] = - m[2][0] * b + m[2][1] * d - m[2][3] * f;
    i[3][3] = + m[2][0] * c - m[2][1] * e + m[2][2] * f;

    d = m[0][0] * i[0][0] + m[0][1] * i[1][0] + m[0][2] * i[2][0] + m[0][3] * i[3][0];
    return i / d;
}
