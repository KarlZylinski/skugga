#include <windows.h>
#include <ctime>
#include "object.h"
#include "types.h"

// Unified build sources.
#include "math.cpp"
#include "window.cpp"
#include "renderer.cpp"
#include "renderer_direct3d.cpp"
#include "primitives.cpp"
#include "color.cpp"
#include "world.cpp"

void create_scaled_box(Vertex* vertices, const Vector3& scale, const Color& color)
{
    for (unsigned i = 0; i < primitives::box_size; ++i)
    {
        vertices[i].position = primitives::box[i] * scale;
        vertices[i].color = color;
    }
}

void create_poop(World* world, RendererState* rs)
{
    Color box_color = color::random();
    Vertex floor_vertices[primitives::box_size];
    create_scaled_box(floor_vertices, {4, 0.3f, 6}, box_color);
    unsigned box_geometry_handle = renderer::load_geometry(rs, floor_vertices, primitives::box_size);
    Object floor_obj = {0};
    floor_obj.geometry_handle = box_geometry_handle;
    floor_obj.world_transform = matrix4x4::identity();
    world::add_object(world, floor_obj);
}

int main()
{
    srand((unsigned)time(0));
    Window window = {0};
    window::init(&window);
    RendererState renderer_state = {0};
    World world = {0};
    renderer::init(&renderer_state, window.handle);
    create_poop(&world, &renderer_state);
    Camera camera = {0};
    renderer::setup_camera(&camera);
    Color clear_color = {0.2f, 0, 0, 1};
    float rot_val = 0;
    float f = -1;

    while(!window.closed)
    {
        window::process_all_messsages();

        Matrix4x4 move = matrix4x4::identity();
        if (window.up)
        {
            f += 0.0005f;
        }
        if (window.down)
        {
            f -= 0.0005f;
        }

        move.w.z = f;
        Matrix4x4 rot = matrix4x4::identity();
        if (window.left)
        {
            rot_val += 0.0005f;
        }
        if (window.right)
        {
            rot_val -= 0.0005f;
        }

        rot.x.x = cos(rot_val);
        rot.x.z = sin(rot_val);
        rot.z.x = -sin(rot_val);
        rot.z.z = cos(rot_val);

        camera.world_transform = rot * move;

        renderer::clear(&renderer_state, clear_color);
        Matrix4x4 view_matrix = matrix4x4::inverse(camera.world_transform);

        for (unsigned i = 0; i < world.num_objects; ++i)
        {
            if (world.objects[i].valid)
                renderer::draw(&renderer_state, world.objects[i].geometry_handle, world.objects[i].world_transform, view_matrix, camera.projection_matrix);
        }

        renderer::present(&renderer_state);
    }

    renderer::shutdown(&renderer_state);
    return 0;
}