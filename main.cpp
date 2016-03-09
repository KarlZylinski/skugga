#include <windows.h>
#include <time.h>
#include "object.h"
#include "types.h"
#include "helpers.h"
#include "memory.h"
#include "windows_window.h"
#include "renderer_direct3d.h"
#include "simulation.h"
#include "keyboard.h"
#include "mouse.h"

int main()
{
    void* temp_memory_block = VirtualAlloc(nullptr, temp_memory::TempMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    temp_memory::init(temp_memory_block, temp_memory::TempMemorySize);
    
    windows::Window window = {};
    windows::window::init(&window);
    RendererState renderer_state = {};
    SimulationState simulation_state = {};
    renderer::init(&renderer_state, window.handle);
    simulation::init(&simulation_state, &renderer_state, &window.state);
    Shader shader = renderer::load_shader(&renderer_state, L"uv_data.shader");
    renderer::set_shader(&renderer_state, &shader);
    /*RenderTarget render_texture = renderer::create_render_texture(&renderer_state, PixelFormat::R8G8B8A8_UINT_NORM);
    renderer::set_render_target(&renderer_state, &render_texture);*/
    camera::set_lightmap_rendering_mode(&simulation_state.camera);



    while(!window.state.closed)
    {
        windows::window::process_all_messsages();
        simulation::simulate(&simulation_state);
        renderer::draw_frame(&renderer_state, simulation_state.world, simulation_state.camera);
        keyboard::end_of_frame();
        mouse::end_of_frame();
    }

    /*{
        Allocator ta = create_temp_allocator();
        Image i = renderer::read_back_texture(&ta, &renderer_state, render_texture);
        File out = {};
        out.data = i.data;
        out.size = image::calc_size(i.pixel_format, i.width, i.height);
        file::write(out, "texture");
    }*/

    renderer::shutdown(&renderer_state);
    return 0;
}