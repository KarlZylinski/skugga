#include <windows.h>
#include <stdio.h>
#include "object.h"
#include "types.h"
#include "helpers.h"
#include "memory.h"
#include "windows_window.h"
#include "renderer_direct3d.h"
#include "simulation.h"
#include "keyboard.h"
#include "mouse.h"
#include "file.h"
#include "lightmapper.h"
#include "test_world.h"

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

int main()
{
    void* temp_memory_block = VirtualAlloc(nullptr, temp_memory::TempMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    temp_memory::init(temp_memory_block, temp_memory::TempMemorySize);
    
    windows::Window window = {};
    windows::window::init(&window);
    Renderer renderer = {};
    renderer.init(window.handle);

    keyboard::init();
    mouse::init();
    window.state.key_released_callback = key_released_callback;
    window.state.key_pressed_callback = key_pressed_callback;
    window.state.mouse_moved_callback = mouse_moved_callback;

    Allocator alloc = create_debug_allocator();

    bool run_lightmapper = true;
    if (run_lightmapper)
    {
        World w = {};
        world::init(&w, &alloc);
        test_world::create_world(&w, &renderer);
        lightmapper::map(w, &renderer);
    }

    Simulation simulation = {};
    simulation.init(&renderer, &alloc);

    //simulation.camera.rotation = quaternion::normalize(quaternion::from_axis_angle({0,1,0}, -PI/2) * quaternion::look_at({0,0,0},{-1,0,0}));
    //quaternion::look_at(vector3::zero, vector3::lookdir);
    renderer.disable_scissor();
    renderer.set_render_target(&renderer.back_buffer);
    RRHandle default_shader = renderer.load_shader(L"shader.shader");
    renderer.set_shader(default_shader);

    while(!window.state.closed)
    {
        windows::window::process_all_messsages();
        simulation.simulate();
        renderer.pre_draw_frame();
        renderer.draw_frame(simulation.world, simulation.camera, DrawLights::DrawLights);
        renderer.present();
        keyboard::end_of_frame();
        mouse::end_of_frame();
    }

    renderer.shutdown();
    return 0;
}