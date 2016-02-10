#include <windows.h>
#include "types.h"
#include "math.cpp"
#include "window.cpp"
#include "renderer.cpp"
#include "renderer_direct3d.cpp"
#include "object.cpp"

Object create_test_triangle(RendererState* rs)
{
    Vertex triangle[]{
        {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
        {0.8f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.1f, -0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},
        {0.9f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.45f, -0.5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
        {-0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}
    };
    unsigned triangle_geometry_handle = renderer::load_geometry(rs, triangle, 6);
    return object::create(triangle_geometry_handle);
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
    Color clear_color = {0.2f, 0, 0, 1};

    Object triangles[4096];
    memset(triangles, 0, sizeof(triangles));

    for (unsigned i = 0; i < 4096; ++i)
    {
        triangles[i] = create_test_triangle(&renderer_state);
        triangles[i].world_transform.w.x = (rand() % 10000)/1000.0f - 5;
        triangles[i].world_transform.w.y = (rand() % 10000)/1000.0f - 5;
        triangles[i].world_transform.w.z = (rand() % 10000)/1000.0f - 5;
    }

    int frames = 0;
    while(!window.closed)
    {
        ++frames;
        window::process_all_messsages();

        if (window.up)
        {
            camera.world_transform.w.z += 0.0005f;
        }
        if (window.down)
        {
            camera.world_transform.w.z -= 0.0005f;
        }
        if (window.left)
        {
            camera.world_transform.w.x -= 0.0005f;
        }
        if (window.right)
        {
            camera.world_transform.w.x += 0.0005f;
        }

        renderer::clear(&renderer_state, clear_color);
        Matrix4x4 view_matrix = matrix4x4::inverse(camera.world_transform);

        for (unsigned i = 0; i < 4096; ++i)
        {
           renderer::draw(&renderer_state, triangles[i].geometry_handle, triangles[i].world_transform, view_matrix, camera.projection_matrix);
        }
        renderer::present(&renderer_state);
    }

    for (unsigned i = 0; i < 4096; ++i)
    {
        object::destroy(&renderer_state, triangles[i]);
    }

    renderer::shutdown(&renderer_state);
    return 0;
}