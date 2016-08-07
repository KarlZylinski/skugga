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

int main()
{
    void* temp_memory_block = VirtualAlloc(nullptr, temp_memory::TempMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    temp_memory::init(temp_memory_block, temp_memory::TempMemorySize);
    
    windows::Window window = {};
    windows::window::init(&window);
    Renderer renderer = {};
    renderer.init(window.handle);
    Simulation simulation = {};
    simulation.init(&renderer, &window.state);

    lightmapper::map(simulation.world, &renderer);

    renderer.set_render_target(&renderer.back_buffer);
    Shader default_shader = renderer.load_shader(L"shader.shader");
    renderer.set_shader(&default_shader);

    while(!window.state.closed)
    {
        windows::window::process_all_messsages();
        simulation.simulate();
        renderer.draw_frame(simulation.world, simulation.camera, DrawLights::DrawLights);
        keyboard::end_of_frame();
        mouse::end_of_frame();
    }

    renderer.shutdown();
    return 0;
}