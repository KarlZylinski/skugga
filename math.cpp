#include "math.h"
#include <cmath>

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2)
{
    return
    {
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
    return {v1.x + v2.x, v1.y + v2.y};
}

void operator+=(Vector2i& v1, const Vector2i& v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
}

Vector2i operator+(const Vector2i& v1, const Vector2i& v2)
{
    return {v1.x + v2.x, v1.y + v2.y};
}

void operator+=(Vector3& v1, const Vector3& v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
}

Vector3 operator-(const Vector3& v)
{
    return {-v.x, -v.y, -v.z};
}

Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

Vector3 operator*(const Vector3& v, float s)
{
    return {v.x * s, v.y * s, v.z * s};
}

Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
    return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

void operator+=(Vector4& v1, const Vector4& v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    v1.w += v2.w;
}

Vector4 operator+(const Vector4& v1, const Vector4& v2)
{
    return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w + v2.w};
}

Vector4 operator*(const Vector4& v, float s)
{
    return {v.x * s, v.y * s, v.z * s, v.w * s};
}

Vector4 operator*(const Vector4& v1, const Vector4& v2)
{
    return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};
}

Vector4 operator*(const Vector4& v, const Matrix4x4& m)
{
    return
    {
        v.x * m.x.x + v.x * m.x.y + v.x * m.x.z + v.x * m.x.w,
        v.y * m.y.x + v.y * m.y.y + v.y * m.y.z + v.y * m.y.w,
        v.z * m.z.x + v.z * m.z.y + v.z * m.z.z + v.z * m.z.w,
        v.x * m.w.x + v.w * m.w.y + v.w * m.w.z + v.w * m.w.w
    };
}

Matrix4x4 operator*(const Matrix4x4& m, float s)
{
    return
    {
        s * m.x.x, s * m.x.y, s * m.x.z, s * m.x.w,
        s * m.y.x, s * m.y.y, s * m.y.z, s * m.y.w,
        s * m.z.x, s * m.z.y, s * m.z.z, s * m.z.w,
        s * m.w.x, s * m.w.y, s * m.w.z, s * m.w.w
    };
}

Vector4 operator-(const Vector4& v1, const Vector4& v2)
{
    return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

namespace matrix4x4
{

Matrix4x4 identity()
{
    return
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

Matrix4x4 inverse(const Matrix4x4& m)
{
    const float* a = &m.x.x;
    float a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3],
        a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7],
        a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11],
        a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15],

        b00 = a00 * a11 - a01 * a10,
        b01 = a00 * a12 - a02 * a10,
        b02 = a00 * a13 - a03 * a10,
        b03 = a01 * a12 - a02 * a11,
        b04 = a01 * a13 - a03 * a11,
        b05 = a02 * a13 - a03 * a12,
        b06 = a20 * a31 - a21 * a30,
        b07 = a20 * a32 - a22 * a30,
        b08 = a20 * a33 - a23 * a30,
        b09 = a21 * a32 - a22 * a31,
        b10 = a21 * a33 - a23 * a31,
        b11 = a22 * a33 - a23 * a32,

    // Calculate the determinant
        det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    det = 1.0f / det;

    Matrix4x4 result;
    float* out = &result.x.x;
    out[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
    out[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
    out[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
    out[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
    out[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
    out[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
    out[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
    out[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
    out[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
    out[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
    out[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
    out[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
    out[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
    out[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
    out[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
    out[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
    return result;
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

Vector3 right(const Matrix4x4& m)
{
    return {m.x.x, m.x.y, m.x.z};
}

Vector3 up(const Matrix4x4& m)
{
    return {m.y.x, m.y.y, m.y.z};
}

} // namespace matrix4x4

namespace vector2
{

bool almost_equal(const Vector2& v1, const Vector2& v2)
{
    return ::almost_equal(v1.x, v2.x) && ::almost_equal(v1.y, v2.y);
}

} // namespace vector3

namespace vector3
{

bool almost_equal(const Vector3& v1, const Vector3& v2)
{
    return ::almost_equal(v1.x, v2.x) && ::almost_equal(v1.y, v2.y) && ::almost_equal(v1.z, v2.z);
}

Vector3 cross(const Vector3& v1, const Vector3& v2)
{
    return 
    {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

float length(const Vector3& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float squared_length(const Vector3& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

float dot(const Vector3& v1, const Vector3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 normalize(const Vector3& v)
{
    float len = length(v);
    return
    {
        v.x / len,
        v.y / len,
        v.z / len
    };
}

Vector3 tangent(const Vector3& v)
{
    Vector3 c1 = cross(v, {0.0, 0.0, 1.0});
    Vector3 c2 = cross(v, {1.0, 0.0, 0.0});
    return normalize(squared_length(c1) > squared_length(c2) ? c1 : c2);
}

Vector3 bitangent(const Vector3& v)
{
    return normalize(cross(tangent(v), v));
}

} // namespace vector3

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

Quaternion identity()
{
    return {0, 0, 0, 1};
}

Quaternion normalize(const Quaternion& q)
{
    float len = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return
    {
        q.x / len,
        q.y / len,
        q.z / len,
        q.w / len
    };
}

Quaternion from_normal(const Vector3& n)
{
    static const Vector3 forward = {0, 0, 1};
    const Vector3 angle = vector3::cross(forward, n);
    float forward_len = vector3::length(forward);
    float w = sqrtf(forward_len * forward_len) + vector3::dot(forward, n);
    return normalize({angle.x, angle.y, angle.z, w});
}

} // namespace quaternion
