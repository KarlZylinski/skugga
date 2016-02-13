struct Camera
{
    Matrix4x4 projection_matrix;
    Matrix4x4 view_matrix;
    Quaternion rotation;
    Vector3 position;
};

namespace camera
{
void recalc_view_matrix(Camera* c)
{
    c->view_matrix = matrix4x4::inverse(matrix4x4::from_rotation_and_translation(c->rotation, c->position));
}

void init(Camera* c)
{
    memset(c, 0, sizeof(Camera));
    float near_plane = 0.01f;
    float far_plane = 1000.0f;
    float fov = 90.0f;
    float aspect = 1.0f;
    float y_scale = 1.0f / tan((3.14f / 180.0f) * fov / 2);
    float x_scale = y_scale / aspect;
    c->projection_matrix = {
        x_scale, 0, 0, 0,
        0, y_scale, 0, 0,
        0, 0, far_plane/(far_plane-near_plane), 1,
        0, 0, (-far_plane * near_plane) / (far_plane - near_plane), 0 
    };
    c->rotation = quaternion::identity();
    c->position.z = -5;
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