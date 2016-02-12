#include "math.h"
#include <cmath>

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2)
{
    return Matrix4x4 {
        m1.x.x * m2.x.x + m1.x.y * m2.y.x + m1.x.z * m2.z.x + m1.x.w * m2.w.x,
        m1.x.x * m2.x.y + m1.x.y * m2.y.y + m1.x.z * m2.z.y + m1.x.w * m2.w.y,
        m1.x.x * m2.x.z + m1.x.y * m2.y.z + m1.x.z * m2.z.z + m1.x.w * m2.w.z,
        m1.x.x * m2.x.w + m1.x.y * m2.y.w + m1.x.z * m2.z.w + m1.x.w * m2.w.w,

        m1.y.x * m2.x.x + m1.y.y * m2.y.x + m1.y.z * m2.z.x + m1.y.w * m2.w.x,
        m1.y.x * m2.x.y + m1.y.y * m2.y.y + m1.y.z * m2.z.y + m1.y.w * m2.w.y,
        m1.y.x * m2.x.z + m1.y.y * m2.y.z + m1.y.z * m2.z.z + m1.y.w * m2.w.z,
        m1.y.x * m2.x.w + m1.y.y * m2.y.w + m1.y.z * m2.z.w + m1.y.w * m2.w.w,

        m1.z.x * m2.x.x + m1.z.y * m2.y.x + m1.z.z * m2.z.x + m1.z.w * m2.w.x,
        m1.z.x * m2.x.y + m1.z.y * m2.y.y + m1.z.z * m2.z.y + m1.z.w * m2.w.y,
        m1.z.x * m2.x.z + m1.z.y * m2.y.z + m1.z.z * m2.z.z + m1.z.w * m2.w.z,
        m1.z.x * m2.x.w + m1.z.y * m2.y.w + m1.z.z * m2.z.w + m1.z.w * m2.w.w,

        m1.w.x * m2.x.x + m1.w.y * m2.y.x + m1.w.z * m2.z.x + m1.w.w * m2.w.x,
        m1.w.x * m2.x.y + m1.w.y * m2.y.y + m1.w.z * m2.z.y + m1.w.w * m2.w.y,
        m1.w.x * m2.x.z + m1.w.y * m2.y.z + m1.w.z * m2.z.z + m1.w.w * m2.w.z,
        m1.w.x * m2.x.w + m1.w.y * m2.y.w + m1.w.z * m2.z.w + m1.w.w * m2.w.w
    };
}

void operator+=(Vector2& v1, const Vector2& v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
}

Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
    return Vector2 {v1.x + v2.x, v1.y + v2.y};
}

void operator+=(Vector2i& v1, const Vector2i& v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
}

Vector2i operator+(const Vector2i& v1, const Vector2i& v2)
{
    return Vector2i {v1.x + v2.x, v1.y + v2.y};
}

void operator+=(Vector3& v1, const Vector3& v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
}

Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
    return Vector3 {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

Vector3 operator*(const Vector3& v, float s)
{
    return Vector3 {v.x * s, v.y * s, v.z * s};
}

Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
    return Vector3 {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

Vector4 operator*(const Vector4& v, float s)
{
    return Vector4 {v.x * s, v.y * s, v.z * s, v.w * s};
}

Vector4 operator*(const Vector4& v1, const Vector4& v2)
{
    return Vector4 {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};
}

Vector4 operator*(const Vector4& v, const Matrix4x4& m)
{
    return Vector4 {
        v.x * m.x.x + v.x * m.x.y + v.x * m.x.z + v.x * m.x.w,
        v.y * m.y.x + v.y * m.y.y + v.y * m.y.z + v.y * m.y.w,
        v.z * m.z.x + v.z * m.z.y + v.z * m.z.z + v.z * m.z.w,
        v.x * m.w.x + v.w * m.w.y + v.w * m.w.z + v.w * m.w.w
    };
}

Matrix4x4 operator*(const Matrix4x4& m, float s)
{
    return Matrix4x4 {
        s * m.x.x, s * m.x.y, s * m.x.z, s * m.x.w,
        s * m.y.x, s * m.y.y, s * m.y.z, s * m.y.w,
        s * m.z.x, s * m.z.y, s * m.z.z, s * m.z.w,
        s * m.w.x, s * m.w.y, s * m.w.z, s * m.w.w
    };
}

Vector4 operator-(const Vector4& v1, const Vector4& v2)
{
    return Vector4 {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

Vector4 operator+(const Vector4& v1, const Vector4& v2)
{
    return Vector4 {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

namespace matrix4x4
{

Matrix4x4 identity()
{
    return Matrix4x4
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

Matrix4x4 inverse(const Matrix4x4& m)
{
    float coef00 = m.z.z * m.w.w - m.w.z * m.z.w;
    float coef02 = m.y.z * m.w.w - m.w.z * m.y.w;
    float coef03 = m.y.z * m.z.w - m.z.z * m.y.w;

    float coef04 = m.z.y * m.w.w - m.w.y * m.z.w;
    float coef06 = m.y.y * m.w.w - m.w.y * m.y.w;
    float coef07 = m.y.y * m.z.w - m.z.y * m.y.w;

    float coef08 = m.z.y * m.w.z - m.w.y * m.z.z;
    float coef10 = m.y.y * m.w.z - m.w.y * m.y.z;
    float coef11 = m.y.y * m.z.z - m.z.y * m.y.z;

    float coef12 = m.z.x * m.w.w - m.w.x * m.z.w;
    float coef14 = m.y.x * m.w.w - m.w.x * m.y.w;
    float coef15 = m.y.x * m.z.w - m.z.x * m.y.w;

    float coef16 = m.z.x * m.w.z - m.w.x * m.z.z;
    float coef18 = m.y.x * m.w.z - m.w.x * m.y.z;
    float coef19 = m.y.x * m.z.z - m.z.x * m.y.z;

    float coef20 = m.z.x * m.w.y - m.w.x * m.z.y;
    float coef22 = m.y.x * m.w.y - m.w.x * m.y.y;
    float coef23 = m.y.x * m.z.y - m.z.x * m.y.y;

    Vector4 fac0 = {coef00, coef00, coef02, coef03};
    Vector4 fac1 = {coef04, coef04, coef06, coef07};
    Vector4 fac2 = {coef08, coef08, coef10, coef11};
    Vector4 fac3 = {coef12, coef12, coef14, coef15};
    Vector4 fac4 = {coef16, coef16, coef18, coef19};
    Vector4 fac5 = {coef20, coef20, coef22, coef23};

    Vector4 vec0 = {m.y.x, m.x.x, m.x.x, m.x.x};
    Vector4 vec1 = {m.y.y, m.x.y, m.x.y, m.x.y};
    Vector4 vec2 = {m.y.z, m.x.z, m.x.z, m.x.z};
    Vector4 vec3 = {m.y.w, m.x.w, m.x.w, m.x.w};

    Vector4 inv0 = {vec1 * fac0 - vec2 * fac1 + vec3 * fac2};
    Vector4 inv1 = {vec0 * fac0 - vec2 * fac3 + vec3 * fac4};
    Vector4 inv2 = {vec0 * fac1 - vec1 * fac3 + vec3 * fac5};
    Vector4 inv3 = {vec0 * fac2 - vec1 * fac4 + vec2 * fac5};

    Vector4 signa = {+1, -1, +1, -1};
    Vector4 signb = {-1, +1, -1, +1};
    Matrix4x4 inverse = {inv0 * signa, inv1 * signb, inv2 * signa, inv3 * signb};

    Vector4 row0 = {inverse.x.x, inverse.y.x, inverse.z.x, inverse.w.x};

    Vector4 dot0 = {m.x * row0};
    float dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

    float one_over_determinant = 1.0f / dot1;

    return inverse * one_over_determinant;
}

Matrix4x4 from_rotation_and_translation(const Quaternion& q, const Vector3& t)
{
    float x = q.x, y = q.y, z = q.z, w = q.w,
        x2 = x + x,
        y2 = y + y,
        z2 = z + z,

        xx = x * x2,
        xy = x * y2,
        xz = x * z2,
        yy = y * y2,
        yz = y * z2,
        zz = z * z2,
        wx = w * x2,
        wy = w * y2,
        wz = w * z2;

    Matrix4x4 out_mat = {0};
    float* out = &out_mat.x.x;
    out[0] = 1 - (yy + zz);
    out[1] = xy + wz;
    out[2] = xz - wy;
    out[3] = 0;
    out[4] = xy - wz;
    out[5] = 1 - (xx + zz);
    out[6] = yz + wx;
    out[7] = 0;
    out[8] = xz + wy;
    out[9] = yz - wx;
    out[10] = 1 - (xx + yy);
    out[11] = 0;
    out[12] = t.x;
    out[13] = t.y;
    out[14] = t.z;
    out[15] = 1;
    return out_mat;
}

} // namespace matrix4x4

namespace vector4
{

float dot(const Vector4& v1, const Vector4& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

} // namespace vector4

namespace quaternion
{

Quaternion rotate_x(const Quaternion& q, float rads)
{
    float adjusted_rads = rads * 0.5f; 
    float bx = sin(adjusted_rads);
    float bw = cos(adjusted_rads);
    return
    {
        q.x * bw + q.w * bx,
        q.y * bw + q.z * bx,
        q.z * bw - q.y * bx,
        q.w * bw - q.x * bx
    };
}

Quaternion rotate_y(const Quaternion& q, float rads)
{
    float adjusted_rads = rads * 0.5f;
    float by = sin(adjusted_rads);
    float bw = cos(adjusted_rads);
    return
    {
        q.x * bw - q.z * by,
        q.y * bw + q.w * by,
        q.z * bw + q.x * by,
        q.w * bw - q.y * by
    };
}

Quaternion rotate_z(const Quaternion& q, float rads)
{
    float adjusted_rads = rads * 0.5f;
    float bz = sin(adjusted_rads);
    float bw = cos(adjusted_rads);
    return
    {
        q.x * bw + q.y * bz,
        q.y * bw - q.x * bz,
        q.z * bw + q.w * bz,
        q.w * bw - q.z * bz
    };
}

} // namespace quaternion
