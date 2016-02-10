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
    camera->projection_matrix = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,1,
        0,0,0,0
    };
    camera->world_transform = matrix4x4::identity();
}
} // namespace renderer
