#include <windows.h>
#include "math.cpp"
#include "window.cpp"
#include "renderer.cpp"
#include "renderer_direct3d.cpp"
#include "object.cpp"

Object create_test_triangle(RendererState* rs)
{
    Vertex triangle[]{
        {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.8f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.1f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.9f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.45f, -0.5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
        {-0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}
    };
    unsigned triangle_geometry_handle = renderer::load_geometry(rs, triangle, 6);
    return object::create(rs, triangle_geometry_handle);
}

int main()
{
    Window window = {0};
    window::init(&window);
    RendererState renderer_state = {0};
    renderer::init(&renderer_state, window.handle);
    Camera camera = {0};
    renderer::setup_camera(&camera);
    camera.world_transform.w.z = -5;
    Object triangle = create_test_triangle(&renderer_state);
    Color clear_color = {0.2f, 0, 0, 1};
    while(!window.closed)
    {
        window::process_all_messsages();
        renderer::clear(&renderer_state, clear_color);
        Matrix4x4 view_matrix = matrix4x4::inverse(camera.world_transform);
        renderer::draw(&renderer_state, triangle.geometry_handle, triangle.world_transform, view_matrix, camera.projection_matrix);
        renderer::present(&renderer_state);
    }
    renderer::shutdown(&renderer_state);
    return 0;
}