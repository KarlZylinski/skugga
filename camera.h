#pragma once

#include "math.h"

struct Camera
{
    Matrix4x4 projection_matrix;
    Quaternion rotation;
    Vector3 position;
};

namespace camera
{

Matrix4x4 calc_view_matrix(const Camera& c);
void set_projection_mode(Camera* c);
void set_lightmap_rendering_mode(Camera* c);

}