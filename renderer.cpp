#include <cmath>

struct Color
{
    float r, g, b, a;
};

struct Vertex
{
    Vector3 position;
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
    float near_plane = 0.2f;
    float far_plane = 100.0f;
    float fov = 90.0f;
    float d2r = 3.14f / 180.0f;
    float aspect = 2.0f;
    float y_scale = 1.0f / tan(d2r * fov / 2);
    float x_scale = y_scale / aspect;
    /*camera->projection_matrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -(far_plane + near_plane) / (far_plane - near_plane), -1,
        0, 0, -2 * far_plane * near_plane / (far_plane - near_plane), 0 
    }; */
    camera->projection_matrix = {
        x_scale, 0, 0, 0,
        0, y_scale, 0, 0,
        0, 0, far_plane/(far_plane-near_plane), 1,
        0, 0, (-far_plane * near_plane) / (far_plane - near_plane), 0 
    };
    camera->world_transform = matrix4x4::identity();
}
} // namespace renderer
