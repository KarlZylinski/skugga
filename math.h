#pragma once

struct Vector3
{
    float x, y, z;
};

struct Vector4
{
    float x, y, z, w;
};

struct Matrix4x4
{
    Vector4 x, y, z, w;
};

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator*(const Vector3& v1, const Vector3& v2);
Vector4 operator*(const Vector4& v, float s);
Vector4 operator*(const Vector4& v1, const Vector4& v2);
Vector4 operator*(const Vector4& v, const Matrix4x4& m);
Matrix4x4 operator*(const Matrix4x4& m, float s);
Vector4 operator-(const Vector4& v1, const Vector4& v2);
Vector4 operator+(const Vector4& v1, const Vector4& v2);

namespace matrix4x4
{

Matrix4x4 identity();
Matrix4x4 inverse(const Matrix4x4& m);

} // namespace matrix4x4

namespace vector4
{

float dot(const Vector4& v1, const Vector4& v2);

} // namespace vector4
