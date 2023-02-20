#pragma once

#include "common.hpp"

union Vec3;
union Vec4;

union Vec3
{
    struct {f32 x, y, z;};
    f32 v[3];

    Vec3() : v{0, 0, 0} {}
    Vec3(f32 x, f32 y, f32 z) : v{x, y, z} {}

    explicit inline Vec3(f32 a) : v{a, a, a} {}
    explicit inline Vec3(const Vec4 &);

    f32 &operator[] (i32 i)
    {
        ASSERT(i >= 0 && i < 3, "index out of range");
        return v[i];
    }

    f32 operator[] (i32 i) const
    {
        ASSERT(i >= 0 && i < 3, "index out of range");
        return v[i];
    }
};

union Vec4
{
    struct {f32 x, y, z, w;};
    f32 v[4];

    Vec4() : v{0, 0, 0, 1} {}
    Vec4(f32 x, f32 y, f32 z, f32 w = 1) : v{x, y, z, w} {}

    explicit inline Vec4(f32 a) : v{a, a, a, a} {}
    explicit inline Vec4(const Vec3 &);

    f32 &operator[] (i32 i)
    {
        ASSERT(i >= 0 && i < 4, "index out of range");
        return v[i];
    }

    f32 operator[] (i32 i) const
    {
        ASSERT(i >= 0 && i < 4, "index out of range");
        return v[i];
    }
};

Vec3::Vec3(const Vec4 &v4) : v{v4.x, v4.y, v4.z} {}
Vec4::Vec4(const Vec3 &v3) : v{v3.x, v3.y, v3.z, 1} {}

inline Vec3 min(const Vec3& a, const Vec3&b) {return {min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)}; }
inline Vec4 min(const Vec4& a, const Vec4&b) {return {min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)}; }
inline Vec3 max(const Vec3& a, const Vec3&b) {return {max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)}; }
inline Vec4 max(const Vec4& a, const Vec4&b) {return {max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)}; }

inline Vec3 operator+ (const Vec3& a, const Vec3& b) {return {a.x + b.x, a.y + b.y, a.z + b.z};}
inline Vec3 operator- (const Vec3& a, const Vec3& b) {return {a.x - b.x, a.y - b.y, a.z - b.z};}
inline Vec3 operator- (const Vec3& a) {return {-a.x , -a.y , -a.z};}
inline Vec3 operator* (const Vec3& a, const Vec3& b) {return {a.x * b.x, a.y * b.y, a.z * b.z};}
inline Vec3 operator/ (const Vec3& a, const Vec3& b) {return {a.x / b.x, a.y / b.y, a.z / b.z};}
inline Vec3 operator* (const Vec3& a, const f32 b) {return a * Vec3(b);}
inline Vec3 operator* (const f32 b, const Vec3& a) {return a * Vec3(b);}
inline Vec3 operator/ (const Vec3& a, const f32 b) {return a / Vec3(b);}

inline Vec3& operator+= (Vec3& a, const Vec3& b) {a = a + b; return a;};
inline Vec3& operator-= (Vec3& a, const Vec3& b) {a = a - b; return a;};
inline Vec3& operator*= (Vec3& a, const Vec3& b) {a = a * b; return a;};
inline Vec3& operator/= (Vec3& a, const Vec3& b) {a = a / b; return a;};
inline Vec3& operator*= (Vec3& a, const f32 b) {a = a * b; return a;};
inline Vec3& operator/= (Vec3& a, const f32 b) {a = a / b; return a;};

inline f32 dot(const Vec3& a, const Vec3& b) {
    Vec3 c = a * b;
    return c.x + c.y + c.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    Vec3 c;
    c.x = a.y * b.z - b.y * a.z;
    c.y = a.z * b.x - b.z * a.x;
    c.z = a.x * b.y - b.x * a.y;
    return c;
}

inline f32 squareMagnitude(const Vec3& a) {return dot(a, a);}
inline f32 magnitude(const Vec3& a) {return sqrtf(dot(a, a));}
inline Vec3 normalize(Vec3 a) {return a / magnitude(a);}

inline Vec4 operator+ (const Vec4& a, const Vec4& b) {return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};}
inline Vec4 operator- (const Vec4& a, const Vec4& b) {return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};}
inline Vec4 operator- (const Vec4& a ) {return { -a.x , -a.y , -a.z , -a.w };}
inline Vec4 operator* (const Vec4& a, const Vec4& b) {return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};}
inline Vec4 operator/ (const Vec4& a, const Vec4& b) {return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};}
inline Vec4 operator* (const Vec4& a, const f32 b) {return a * Vec4(b);}
inline Vec4 operator* (const f32 b, const Vec4& a) {return a * Vec4(b);}
inline Vec4 operator/ (const Vec4& a, const f32 b) {return a / Vec4(b);}

inline Vec4& operator+= (Vec4& a, const Vec4& b) {a = a + b; return a;};
inline Vec4& operator-= (Vec4& a, const Vec4& b) {a = a - b; return a;};
inline Vec4& operator*= (Vec4& a, const Vec4& b) {a = a * b; return a;};
inline Vec4& operator/= (Vec4& a, const Vec4& b) {a = a / b; return a;};
inline Vec4& operator*= (Vec4& a, const f32 b) {a = a * b; return a;};
inline Vec4& operator/= (Vec4& a, const f32 b) {a = a / b; return a;};

inline f32 dot(const Vec4& a, const Vec4& b) {
    Vec4 c = a * b;
    return c.x + c.y + c.z + c.w;
}

inline Vec4 cross(const Vec4& a, const Vec4& b) {
    Vec4 c;
    c.x = a.y * b.z - b.y * a.z;
    c.y = a.z * b.x - b.z * a.x;
    c.z = a.x * b.y - b.x * a.y;
    return c;
}

//Rohan: added magnitude & normalize & unary minus & mat4 lookAt
inline f32 squareMagnitude(const Vec4& a) {return dot(a, a);}
inline f32 magnitude(const Vec4& a) {return sqrtf(dot(a, a));}
inline Vec4 normalize(Vec4 a) {return a / magnitude(a);}
