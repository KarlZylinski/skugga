#pragma once

#include "math.h"

struct Camera
{
    Matrix4x4 projection_matrix;
    Matrix4x4 view_matrix;
    Quaternion rotation;
    Vector3 position;
};

namespace camera
{

void set_projection_mode(Camera* c);
void set_lightmap_rendering_mode(Camera* c);
void set_position(Camera* c, const Vector3& position);
void set_rotation(Camera* c, const Quaternion& rotation);
void move(Camera* c, const Vector3& distance);

}