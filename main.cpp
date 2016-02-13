#include <windows.h>
#include <ctime>
#include "object.h"
#include "types.h"

// Unified build sources.
#include "memory.cpp"
#include "math.cpp"
#include "window.cpp"
#include "renderer.cpp"
#include "renderer_direct3d.cpp"
#include "primitives.cpp"
#include "color.cpp"
#include "world.cpp"
#include "keyboard.cpp"
#include "mouse.cpp"
#include "camera.cpp"
#include "simulation.cpp"

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
    unsigned temp_memory_size = 1024 * 1024 * 100;
    void* temp_memory_block = VirtualAlloc(nullptr, temp_memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    temp_memory::init(temp_memory_block, temp_memory_size);
    srand((unsigned)time(0));
    Window window = {0};
    window::init(&window);
    memset(&keyboard_state, 0, sizeof(Keyboard));
    memset(&mouse_state, 0, sizeof(Mouse));
    window.key_released_callback = key_released_callback;
    window.key_pressed_callback = key_pressed_callback;
    window.mouse_moved_callback = mouse_moved_callback;
    RendererState renderer_state = {0};
    SimulationState simulation_state = {0};
    renderer::init(&renderer_state, window.handle);
    simulation::init(&simulation_state, &renderer_state);
    camera::init(&simulation_state.camera);
    Color clear_color = {0.2f, 0, 0, 1};

    while(!window.closed)
    {
        window::process_all_messsages();
        simulation::simulate(&simulation_state);
        renderer::clear(&renderer_state, clear_color);

        for (unsigned i = 0; i < simulation_state.world.num_objects; ++i)
        {
            if (simulation_state.world.objects[i].valid)
                renderer::draw(&renderer_state, simulation_state.world.objects[i].geometry_handle, simulation_state.world.objects[i].world_transform, simulation_state.camera.view_matrix, simulation_state.camera.projection_matrix);
        }

        keyboard::end_of_frame();
        mouse::end_of_frame();
        renderer::present(&renderer_state);
    }

    renderer::shutdown(&renderer_state);
    return 0;
}