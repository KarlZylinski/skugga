#include "camera.h"
#include <math.h>

namespace camera
{

void recalc_view_matrix(Camera* c)
{
    c->view_matrix = matrix4x4::inverse(matrix4x4::from_rotation_and_translation(c->rotation, c->position));
}

void set_projection_mode(Camera* c)
{
    float near_plane = 0.01f;
    float far_plane = 1000.0f;
    float fov = 90.0f;
    float aspect = 1.0f;
    float y_scale = 1.0f / tanf((3.14f / 180.0f) * fov / 2);
    float x_scale = y_scale / aspect;
    c->projection_matrix = {
        x_scale, 0, 0, 0,
        0, y_scale, 0, 0,
        0, 0, far_plane/(far_plane-near_plane), 1,
        0, 0, (-far_plane * near_plane) / (far_plane - near_plane), 0 
    };
    c->rotation = quaternion::identity();
    recalc_view_matrix(c);
}

void set_lightmap_rendering_mode(Camera* c)
{
    c->projection_matrix = {
        -2, 0, 0, 0,
        0, 2, 0, 0,
        0, 0, 1, 0,
        1, -1, 0, 1 
    };
    c->rotation = quaternion::identity();
    recalc_view_matrix(c);
}

void set_position(Camera* c, const Vector3& position)
{
    c->position = position;
    recalc_view_matrix(c);
}

void set_rotation(Camera* c, const Quaternion& rotation)
{
    c->rotation = rotation;
    recalc_view_matrix(c);
}

void move(Camera* c, const Vector3& distance)
{
    c->position += distance;
    recalc_view_matrix(c);
}
}