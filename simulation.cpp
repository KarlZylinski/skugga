#include "simulation.h"
#include "window_state.h"
#include "mesh.h"
#include "renderer_direct3d.h"
#include "memory.h"
#include "obj.h"
#include "keyboard.h"
#include "mouse.h"

static void create_scaled_box(World* world, RendererState* rs, const Mesh& m, const Vector3& scale, const Vector3& pos, const Color& color)
{
    Vertex* scaled_vertices = m.vertices.clone_raw();
    memcpy(scaled_vertices, m.vertices.data, m.vertices.num * sizeof(Vertex));

    for (unsigned i = 0; i < m.vertices.num; ++i)
    {
        scaled_vertices[i].position = scaled_vertices[i].position * scale;
    }

    unsigned box_geometry_handle = renderer::load_geometry(rs, scaled_vertices, m.vertices.num, m.indices.data, m.indices.num);
    Object floor_obj = {0};
    floor_obj.geometry_handle = box_geometry_handle;
    floor_obj.world_transform = matrix4x4::identity();
    memcpy(&floor_obj.world_transform.w.x, &pos.x, sizeof(Vector3));
    world::add_object(world, floor_obj);
}

static void create_world(World* world, RendererState* rs)
{
    Allocator ta = create_temp_allocator();
    LoadedMesh lm = obj::load(&ta, "box.wobj");

    if (!lm.valid)
        return;

    float floor_width = 6;
    float floor_depth = 8;
    float floor_thickness = 0.3f;
    float floor_to_cieling = 2;
    float pillar_width = 0.4f;
    create_scaled_box(world, rs, lm.mesh, {floor_width, floor_thickness, floor_depth}, {0, 0, 0}, color::random());
    create_scaled_box(world, rs, lm.mesh, {pillar_width, floor_to_cieling, pillar_width}, {-1, (floor_thickness + floor_to_cieling) / 2, 1}, color::random());
    create_scaled_box(world, rs, lm.mesh, {pillar_width, floor_to_cieling, pillar_width}, {-1, (floor_thickness + floor_to_cieling) / 2, -1}, color::random());
    create_scaled_box(world, rs, lm.mesh, {floor_width, floor_thickness, floor_depth}, {0, floor_thickness + floor_to_cieling, 0}, color::random());
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

namespace simulation
{

void init(SimulationState* ss, RendererState* rs, WindowState* window_state)
{
    keyboard::init();
    mouse::init();
    window_state->key_released_callback = key_released_callback;
    window_state->key_pressed_callback = key_pressed_callback;
    window_state->mouse_moved_callback = mouse_moved_callback;
    create_world(&ss->world, rs);
    camera::set_projection_mode(&ss->camera);
}

void simulate(SimulationState* ss)
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

    Quaternion rotation = ss->camera.rotation;

    if (mouse::delta().x != 0 || mouse::delta().y != 0)
    {
        rotation = quaternion::rotate_y(rotation, mouse::delta().x * 0.001f);
        rotation = quaternion::rotate_x(rotation, mouse::delta().y * 0.001f);
    }

    camera::set_rotation(&ss->camera, rotation);
    Matrix4x4 camera_test_mat = matrix4x4::from_rotation_and_translation(ss->camera.rotation, {0,0,0});
    Matrix4x4 movement_rotated = move * camera_test_mat;
    camera::move(&ss->camera, (*(Vector3*)&movement_rotated.w.x));
}

}