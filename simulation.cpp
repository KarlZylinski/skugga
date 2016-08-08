#include "simulation.h"
#include "window_state.h"
#include "mesh.h"
#include "renderer_direct3d.h"
#include "memory.h"
#include "obj.h"
#include "keyboard.h"
#include "mouse.h"

static Object create_scaled_box(Renderer* renderer, const Mesh& m, const Vector3& scale, const Vector3& pos, const Color& color, unsigned id)
{
    Vertex* scaled_vertices = m.vertices.clone_raw();
    memcpy(scaled_vertices, m.vertices.data, m.vertices.num * sizeof(Vertex));

    for (unsigned i = 0; i < m.vertices.num; ++i)
    {
        scaled_vertices[i].position = scaled_vertices[i].position * scale;
        scaled_vertices[i].color = color;
    }

    RRHandle box_geometry_handle = renderer->load_geometry(scaled_vertices, m.vertices.num, m.indices.data, m.indices.num);
    Object obj = {0};
    obj.geometry_handle = box_geometry_handle;
    obj.world_transform = matrix4x4::identity();
    obj.id = id;
    memcpy(&obj.world_transform.w.x, &pos.x, sizeof(Vector3));

    static wchar lightmap_filename[256];
    wsprintf(lightmap_filename, L"%d.data", id);
    Allocator ta = create_temp_allocator();
    RRHandle lightmap_handle = renderer->load_texture(&ta, lightmap_filename);

    if (IsValidRRHandle(lightmap_handle))
    {
        obj.lightmap_handle = lightmap_handle;
    }

    return obj;
}

static void create_world(World* world, Renderer* renderer)
{
    Allocator ta = create_temp_allocator();
    LoadedMesh lm = obj::load(&ta, L"box.wobj");

    if (!lm.valid)
        return;

    float floor_width = 6;
    float floor_depth = 8;
    float floor_thickness = 0.3f;
    float floor_to_cieling = 2;
    float pillar_width = 0.4f;

    world::add_object(world, create_scaled_box(renderer, lm.mesh, {floor_width, floor_thickness, floor_depth}, {0, 0, 0}, color::random(), 4));
    world::add_object(world, create_scaled_box(renderer, lm.mesh, {pillar_width, floor_to_cieling, pillar_width}, {-1, (floor_thickness + floor_to_cieling) / 2, 1}, color::random(), 12));
    world::add_object(world, create_scaled_box(renderer, lm.mesh, {pillar_width, floor_to_cieling, pillar_width}, {-1, (floor_thickness + floor_to_cieling) / 2, -1}, color::random(), 123));
    world::add_object(world, create_scaled_box(renderer, lm.mesh, {floor_width, floor_thickness, floor_depth}, {0, floor_thickness + floor_to_cieling, 0}, color::random(), 145));

    //world::add_object(world, create_scaled_box(renderer, lm.mesh, {5,5,5}, {0, 0, 0}, color::random(), 145));

    //world::add_object(world, create_scaled_box(renderer, lm.mesh, {1, 1, 1}, {-10, 0, 0}, color::random()));
}

static void key_pressed_callback(Key key)
{
    keyboard::pressed(key);
}

static void key_released_callback(Key key)
{
    keyboard::released(key);
}

static void mouse_moved_callback(const Vector2i& delta)
{
    mouse::add_delta(delta);
}

// Simulation impl.

void Simulation::init(Renderer* renderer, WindowState* window_state)
{
    keyboard::init();
    mouse::init();
    window_state->key_released_callback = key_released_callback;
    window_state->key_pressed_callback = key_pressed_callback;
    window_state->mouse_moved_callback = mouse_moved_callback;
    create_world(&world, renderer);

    {
        Allocator ta = create_temp_allocator();
        LoadedMesh lm = obj::load(&ta, L"box.wobj");

        if (lm.valid)
        {
            create_light(renderer, &lm.mesh, {15, 20, -19});
        }
    }

    camera::set_projection_mode(&camera);
}

void Simulation::simulate()
{
    Matrix4x4 move = matrix4x4::identity();

    if (keyboard::is_held(Key::W))
    {
        move.w.z += 0.0005f;
    }
    if (keyboard::is_held(Key::S))
    {
        move.w.z -= 0.0005f;
    }
    if (keyboard::is_held(Key::A))
    {
        move.w.x -= 0.0005f;
    }
    if (keyboard::is_held(Key::D))
    {
        move.w.x += 0.0005f;
    }

    Quaternion rotation = camera.rotation;

    if (mouse::delta().x != 0 || mouse::delta().y != 0)
    {
        rotation = quaternion::rotate_y(rotation, mouse::delta().x * 0.001f);
        rotation = quaternion::rotate_x(rotation, mouse::delta().y * 0.001f);
    }

    camera.rotation = rotation;
    Matrix4x4 camera_test_mat = matrix4x4::from_rotation_and_translation(camera.rotation, {0,0,0});
    Matrix4x4 movement_rotated = move * camera_test_mat;
    camera.position += *(Vector3*)&movement_rotated.w.x;
}

void Simulation::create_light(Renderer* renderer, Mesh* mesh, const Vector3& position)
{
    for (unsigned i = 0; i < mesh->vertices.num; ++i)
    {
        mesh->vertices[i].light_emittance = 1.0f;
    }

    world::add_light(&world, create_scaled_box(renderer, *mesh, {10, 10, 10}, position, {0,1,1,1}, 10000));
}
