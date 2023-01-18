#include "math.hpp"

//
// vec4
//

float dot(const vec4& a, const vec4& b)
{
    vec4 c = a * b;
    return c.x + c.y + c.z + c.w;
}

vec4 cross(const vec4& a, const vec4& b)
{
    vec4 c;
    c.x = a.y * b.z - b.y * a.z;
    c.y = a.z * b.x - b.z * a.x;
    c.z = a.x * b.y - b.x * a.y;
    return c;
}

vec4 operator+ (const vec4& a, const vec4& b) {return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};}
vec4 operator- (const vec4& a, const vec4& b) {return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};}
vec4 operator* (const vec4& a, const vec4& b) {return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};}
vec4 operator/ (const vec4& a, const vec4& b) {return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};}
vec4 operator* (const vec4& a, const float b) {return a * vec4(b);}
vec4 operator* (const float b, const vec4& a) {return a * vec4(b);}
vec4 operator/ (const vec4& a, const float b) {return a / vec4(b);}

vec4 operator* (const vec4& a, const mat4& b)
{
    vec4 x, y, z, w;
    x = a.x * b[0];
    y = a.y * b[1];
    z = a.z * b[2];
    w = a.w * b[3];
    return x + y + z + w;
}

vec4& operator+= (vec4& a, const vec4& b) {a = a + b; return a;};
vec4& operator-= (vec4& a, const vec4& b) {a = a - b; return a;};
vec4& operator*= (vec4& a, const vec4& b) {a = a * b; return a;};
vec4& operator/= (vec4& a, const vec4& b) {a = a / b; return a;};
vec4& operator*= (vec4& a, const float b) {a = a * b; return a;};
vec4& operator/= (vec4& a, const float b) {a = a / b; return a;};

//
// mat4
//

mat4 mat4_identity()
{
    return {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };
}

mat4 mat4_translation(float x, float y, float z)
{
    return {
        {1, 0, 0, x},
        {0, 1, 0, y},
        {0, 0, 1, z},
        {0, 0, 0, 1},
    };
}

mat4 mat4_perspective(float zn, float zf, float fov, float ar)
{
    float zr = zn - zf;
    float a = DEG2RAD(fov / 2);
    float y = 1 / tanf(a);
    float x = y / ar;
    float p = - (zn + zf) / zr;
    float q = 2 * zn * zf / zr;
    return {
        {x, 0, 0, 0},
        {0, y, 0, 0},
        {0, 0, p, q},
        {0, 0, 1, 0},
    };
}

mat4 mat4_rotation_x(float a) {return mat4_rotation_x(cosf(a), sinf(a));}
mat4 mat4_rotation_y(float a) {return mat4_rotation_y(cosf(a), sinf(a));}
mat4 mat4_rotation_z(float a) {return mat4_rotation_z(cosf(a), sinf(a));}

mat4 mat4_rotation_x(float c, float s)
{
    return {
        {1, 0,  0, 0},
        {0, c, -s, 0},
        {0, s,  c, 0},
        {0, 0,  0, 1},
    };
}

mat4 mat4_rotation_y(float c, float s)
{
    return {
        {c, 0, -s, 0},
        {0, 1,  0, 0},
        {s, 0,  c, 0},
        {0, 0,  0, 1},
    };
}

mat4 mat4_rotation_z(float c, float s)
{
    return {
        {c, -s, 0, 0},
        {s,  c, 0, 0},
        {0,  0, 1, 0},
        {0,  0, 0, 1},
    };
}

mat4 operator+ (const mat4& a, const mat4& b) {return {a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]};}
mat4 operator- (const mat4& a, const mat4& b) {return {a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]};}
mat4 operator* (const mat4& a, const float b) {return {a[0] * b   , a[1] * b   , a[2] * b   , a[3] * b   };}
mat4 operator* (const float b, const mat4& a) {return {a[0] * b   , a[1] * b   , a[2] * b   , a[3] * b   };}
mat4 operator/ (const mat4& a, const float b) {return {a[0] / b   , a[1] / b   , a[2] / b   , a[3] / b   };}

mat4 operator* (const mat4& a, const mat4& b)
{
    mat4 c;
    c[0] = a[0] * b;
    c[1] = a[1] * b;
    c[2] = a[2] * b;
    c[3] = a[3] * b;
    return c;
}

vec4 operator* (const mat4& a, const vec4& b)
{
    return {
        dot(a[0], b),
        dot(a[1], b),
        dot(a[2], b),
        dot(a[3], b),
    };
}

mat4& operator+= (mat4& a, const mat4& b) {a = a + b; return a;}
mat4& operator-= (mat4& a, const mat4& b) {a = a - b; return a;}
mat4& operator*= (mat4& a, const mat4& b) {a = a * b; return a;}
mat4& operator*= (mat4& a, const float b) {a = a * b; return a;}
mat4& operator/= (mat4& a, const float b) {a = a / b; return a;}
