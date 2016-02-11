#include <windows.h>
#include <ctime>
#include "types.h"
#include "math.cpp"
#include "window.cpp"
#include "renderer.cpp"
#include "renderer_direct3d.cpp"
#include "object.cpp"
#include "primitives.cpp"
#include "color.cpp"
#include "world.cpp"

void create_poop(World* world, RendererState* rs)
{
    Vertex vertices[primitives::box_size];
    Color box_color = color::random();

    for (unsigned i = 0; i < primitives::box_size; ++i)
    {
        if (i % 6 == 0)
        {
            box_color = color::random();
        }
        vertices[i].position = primitives::box[i] * 0.05f;
        vertices[i].color = box_color;
    }

    unsigned box_geometry_handle = renderer::load_geometry(rs, vertices, primitives::box_size);

    for (unsigned i = 0; i < 256; ++i)
    {
        Object o = object::create(box_geometry_handle);
        o.world_transform.w.x = (rand() % 1000)/1000.0f - 0.5f;
        o.world_transform.w.y = (rand() % 1000)/1000.0f - 0.5f;
        o.world_transform.w.z = (rand() % 1000)/1000.0f - 0.5f;
        world::add_object(world, o);
    }
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
           renderer::draw(&renderer_state, world.objects[i].geometry_handle, world.objects[i].world_transform, view_matrix, camera.projection_matrix);
        }

        renderer::present(&renderer_state);
    }

    for (unsigned i = 0; i < world.num_objects; ++i)
    {
        object::destroy(&renderer_state, world.objects[i]);
    }

    renderer::shutdown(&renderer_state);
    return 0;
}