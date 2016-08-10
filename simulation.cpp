#include "simulation.h"
#include "window_state.h"
#include "mesh.h"
#include "renderer_direct3d.h"
#include "memory.h"
#include "obj.h"
#include "keyboard.h"
#include "mouse.h"
#include "test_world.h"

// Simulation impl.

void Simulation::init(Renderer* renderer, Allocator* alloc)
{
    world::init(&world, alloc);
    test_world::create_world(&world, renderer);
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
