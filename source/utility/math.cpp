#include "math.hpp"

//
// vec4
//

f32 squareMagnitude(const Vec4& a) {
    return (a.x * a.x + a.y * a.y + a.z * a.z);
}

f32 magnitude(const Vec4& a){
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

f32 dot(const Vec4& a, const Vec4& b)
{
    Vec4 c = a * b;
    return c.x + c.y + c.z + c.w;
}

Vec4 normalize(Vec4 a){
    return a / magnitude(a);
}

Vec4 cross(const Vec4& a, const Vec4& b)
{
    Vec4 c;
    c.x = a.y * b.z - b.y * a.z;
    c.y = a.z * b.x - b.z * a.x;
    c.z = a.x * b.y - b.x * a.y;
    return c;
}

Vec4 operator+ (const Vec4& a, const Vec4& b) {return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};}
Vec4 operator- (const Vec4& a, const Vec4& b) {return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};}
Vec4 operator- (const Vec4& a ) {return { -a.x , -a.y , -a.z , -a.w };}
Vec4 operator* (const Vec4& a, const Vec4& b) {return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};}
Vec4 operator/ (const Vec4& a, const Vec4& b) {return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};}
Vec4 operator* (const Vec4& a, const f32 b) {return a * Vec4(b);}
Vec4 operator* (const f32 b, const Vec4& a) {return a * Vec4(b);}
Vec4 operator/ (const Vec4& a, const f32 b) {return a / Vec4(b);}

Vec4 operator* (const Vec4& a, const Mat4& b)
{
    Vec4 x, y, z, w;
    x = a.x * b[0];
    y = a.y * b[1];
    z = a.z * b[2];
    w = a.w * b[3];
    return x + y + z + w;
}

Vec4& operator+= (Vec4& a, const Vec4& b) {a = a + b; return a;};
Vec4& operator-= (Vec4& a, const Vec4& b) {a = a - b; return a;};
Vec4& operator*= (Vec4& a, const Vec4& b) {a = a * b; return a;};
Vec4& operator/= (Vec4& a, const Vec4& b) {a = a / b; return a;};
Vec4& operator*= (Vec4& a, const f32 b) {a = a * b; return a;};
Vec4& operator/= (Vec4& a, const f32 b) {a = a / b; return a;};

//
// mat4
//

Mat4 mat4Identity()
{
    return {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };
}

Mat4 mat4Translation(f32 x, f32 y, f32 z)
{
    return {
        {1, 0, 0, x},
        {0, 1, 0, y},
        {0, 0, 1, z},
        {0, 0, 0, 1},
    };
}

Mat4 mat4Perspective(f32 zn, f32 zf, f32 fov, f32 ar)
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

Mat4 mat4RotationX(f32 a) {return mat4RotationX(cosf(a), sinf(a));}
Mat4 mat4RotationY(f32 a) {return mat4RotationY(cosf(a), sinf(a));}
Mat4 mat4RotationZ(f32 a) {return mat4RotationZ(cosf(a), sinf(a));}

Mat4 mat4RotationX(f32 c, f32 s)
{
    return {
        {1, 0,  0, 0},
        {0, c, -s, 0},
        {0, s,  c, 0},
        {0, 0,  0, 1},
    };
}

Mat4 mat4RotationY(f32 c, f32 s)
{
    return {
        {c, 0, -s, 0},
        {0, 1,  0, 0},
        {s, 0,  c, 0},
        {0, 0,  0, 1},
    };
}

Mat4 mat4RotationZ(f32 c, f32 s)
{
    return {
        {c, -s, 0, 0},
        {s,  c, 0, 0},
        {0,  0, 1, 0},
        {0,  0, 0, 1},
    };
}

//lookAt function
Mat4 lookAt(Vec4 position, Vec4 front, Vec4 up){
    position.w = 0;
    Vec4 f = normalize(front);
    Vec4 r = normalize(cross(up,f));
    Vec4 u = cross(f,r);
    Mat4 view={
        Vec4(r.x, r.y, r.z, -dot(r,position)),
        Vec4(u.x, u.y, u.z, -dot(u,position)),
        Vec4(f.x, f.y, f.z, -dot(f,position)),
        Vec4(  0,   0,   0,                1),
    };
    return view;
}

Mat4 operator+ (const Mat4& a, const Mat4& b) {return {a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]};}
Mat4 operator- (const Mat4& a, const Mat4& b) {return {a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]};}
Mat4 operator* (const Mat4& a, const f32 b) {return {a[0] * b, a[1] * b, a[2] * b, a[3] * b};}
Mat4 operator* (const f32 b, const Mat4& a) {return {a[0] * b, a[1] * b, a[2] * b, a[3] * b};}
Mat4 operator/ (const Mat4& a, const f32 b) {return {a[0] / b, a[1] / b, a[2] / b, a[3] / b};}

Mat4 operator* (const Mat4& a, const Mat4& b)
{
    Mat4 c;
    c[0] = a[0] * b;
    c[1] = a[1] * b;
    c[2] = a[2] * b;
    c[3] = a[3] * b;
    return c;
}

Vec4 operator* (const Mat4& a, const Vec4& b)
{
    return {
        dot(a[0], b),
        dot(a[1], b),
        dot(a[2], b),
        dot(a[3], b),
    };
}

Mat4& operator+= (Mat4& a, const Mat4& b) {a = a + b; return a;}
Mat4& operator-= (Mat4& a, const Mat4& b) {a = a - b; return a;}
Mat4& operator*= (Mat4& a, const Mat4& b) {a = a * b; return a;}
Mat4& operator*= (Mat4& a, const f32 b) {a = a * b; return a;}
Mat4& operator/= (Mat4& a, const f32 b) {a = a / b; return a;}
