#include <cmath>
#include "color.h"

struct Vertex
{
    Vector3 position;
    Vector3 normal;
    Color color;
};

struct Camera
{
    Matrix4x4 projection_matrix;
    Matrix4x4 world_transform;
};

namespace renderer
{
void setup_camera(Camera* camera)
{
    float near_plane = 0.01f;
    float far_plane = 1000.0f;
    float fov = 90.0f;
    float aspect = 1.0f;
    float y_scale = 1.0f / tan((3.14f / 180.0f) * fov / 2);
    float x_scale = y_scale / aspect;
    camera->projection_matrix = {
        x_scale, 0, 0, 0,
        0, y_scale, 0, 0,
        0, 0, far_plane/(far_plane-near_plane), 1,
        0, 0, (-far_plane * near_plane) / (far_plane - near_plane), 0 
    };
    camera->world_transform = matrix4x4::identity();
}
} // namespace renderer
