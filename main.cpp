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
#include "keyboard.cpp"
#include "mouse.cpp"

void create_scaled_box(World* world, RendererState* rs, const Vector3& scale, const Vector3& pos, const Color& color)
{
    Vertex vertices[primitives::box_size];

    for (unsigned i = 0; i < primitives::box_size; ++i)
    {
        vertices[i].position = primitives::box[i] * scale;
        vertices[i].normal = primitives::box_normals[i];
        vertices[i].color = color;
    }

    unsigned box_geometry_handle = renderer::load_geometry(rs, vertices, primitives::box_size);
    Object floor_obj = {0};
    floor_obj.geometry_handle = box_geometry_handle;
    floor_obj.world_transform = matrix4x4::identity();
    memcpy(&floor_obj.world_transform.w.x, &pos.x, sizeof(Vector3));
    world::add_object(world, floor_obj);
}

void create_world(World* world, RendererState* rs)
{
    float floor_width = 6;
    float floor_depth = 8;
    float floor_thickness = 0.3f;
    float floor_to_cieling = 2;
    float pillar_width = 0.4f;
    create_scaled_box(world, rs, {floor_width, floor_thickness, floor_depth}, {0, 0, 0}, color::random());
    create_scaled_box(world, rs, {pillar_width, floor_to_cieling, pillar_width}, {-1, (floor_thickness + floor_to_cieling) / 2, 1}, color::random());
    create_scaled_box(world, rs, {pillar_width, floor_to_cieling, pillar_width}, {-1, (floor_thickness + floor_to_cieling) / 2, -1}, color::random());
    create_scaled_box(world, rs, {floor_width, floor_thickness, floor_depth}, {0, floor_thickness + floor_to_cieling, 0}, color::random());
}

void key_pressed_callback(Key key)
{
    keyboard_state.pressed[(unsigned)key] = true;
    keyboard_state.held[(unsigned)key] = true;
}

void key_released_callback(Key key)
{
    keyboard_state.released[(unsigned)key] = true;
    keyboard_state.held[(unsigned)key] = false;
}

void mouse_moved_callback(const Vector2i& delta)
{
    mouse_state.delta += delta;
}

int main()
{
    srand((unsigned)time(0));
    Window window = {0};
    window::init(&window);
    memset(&keyboard_state, 0, sizeof(Keyboard));
    memset(&mouse_state, 0, sizeof(Mouse));
    window.key_released_callback = key_released_callback;
    window.key_pressed_callback = key_pressed_callback;
    window.mouse_moved_callback = mouse_moved_callback;
    RendererState renderer_state = {0};
    World world = {0};
    renderer::init(&renderer_state, window.handle);
    create_world(&world, &renderer_state);
    Camera camera = {0};
    renderer::setup_camera(&camera);
    Color clear_color = {0.2f, 0, 0, 1};

    Vector3 t = {0, 0, -5};
    Quaternion rotation = {0, 0, 0, 1};

    while(!window.closed)
    {
        window::process_all_messsages();

        Matrix4x4 move = matrix4x4::identity();

        if (keyboard::held(Key::W))
        {
            move.w.z += 0.0005f;
        }
        if (keyboard::held(Key::S))
        {
            move.w.z -= 0.0005f;
        }
        if (keyboard::held(Key::A))
        {
            move.w.x -= 0.0005f;
        }
        if (keyboard::held(Key::D))
        {
            move.w.x += 0.0005f;
        }

        if (mouse::delta().x != 0 || mouse::delta().y != 0)
        {
            rotation = quaternion::rotate_y(rotation, mouse::delta().x * 0.001f);
            rotation = quaternion::rotate_x(rotation, mouse::delta().y * 0.001f);
        }

        Matrix4x4 camera_test_mat = matrix4x4::from_rotation_and_translation(rotation, Vector3{0,0,0});

        Matrix4x4 movement_rotated = move * camera_test_mat;

        t += *(Vector3*)&movement_rotated.w.x;

        Matrix4x4 camera_mat = matrix4x4::from_rotation_and_translation(rotation, t);

        camera.world_transform = camera_mat;

        renderer::clear(&renderer_state, clear_color);
        Matrix4x4 view_matrix = matrix4x4::inverse(camera.world_transform);

        for (unsigned i = 0; i < world.num_objects; ++i)
        {
            if (world.objects[i].valid)
                renderer::draw(&renderer_state, world.objects[i].geometry_handle, world.objects[i].world_transform, view_matrix, camera.projection_matrix);
        }

        keyboard::end_of_frame();
        mouse::end_of_frame();
        renderer::present(&renderer_state);
    }

    renderer::shutdown(&renderer_state);
    return 0;
}