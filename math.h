#pragma once

#include "types.h"

struct Vector2
{
    float x, y;
};

struct Vector2i
{
    int x, y;
};

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

struct Quaternion
{
    float x, y, z, w;
};

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
void operator+=(Vector2& v1, const Vector2& v2);
Vector2 operator+(const Vector2& v1, const Vector2& v2);
void operator+=(Vector2i& v1, const Vector2i& v2);
Vector2i operator+(const Vector2i& v1, const Vector2i& v2);
void operator+=(Vector3& v1, const Vector3& v2);
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator*(const Vector3& v1, const Vector3& v2);
void operator+=(Vector4& v1, const Vector4& v2);
Vector4 operator+(const Vector4& v1, const Vector4& v2);
Vector4 operator*(const Vector4& v, float s);
Vector4 operator*(const Vector4& v1, const Vector4& v2);
Vector4 operator*(const Vector4& v, const Matrix4x4& m);
Matrix4x4 operator*(const Matrix4x4& m, float s);
Vector4 operator-(const Vector4& v1, const Vector4& v2);

namespace matrix4x4
{

Matrix4x4 identity();
Matrix4x4 inverse(const Matrix4x4& m);
Matrix4x4 from_rotation_and_translation(const Quaternion& q, const Vector3& t);

} // namespace matrix4x4

namespace vector2
{

bool almost_equal(const Vector2& v1, const Vector2& v2);

} // namespace vector2

namespace vector3
{

bool almost_equal(const Vector3& v1, const Vector3& v2);

} // namespace vector3

namespace vector4
{

float dot(const Vector4& v1, const Vector4& v2);

} // namespace vector4

namespace quaternion
{

Quaternion rotate_x(const Quaternion& q, float rads);
Quaternion rotate_y(const Quaternion& q, float rads);
Quaternion rotate_z(const Quaternion& q, float rads);
Quaternion identity();

} // namespace quaternion
