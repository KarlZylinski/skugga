#include <windows.h>
#include <time.h>
#include "object.h"
#include "types.h"
#include "helpers.h"

// This must be last of the includes!
#include "build_include.h"

int main()
{
    void* temp_memory_block = VirtualAlloc(nullptr, temp_memory::TempMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    temp_memory::init(temp_memory_block, temp_memory::TempMemorySize);
    
    WindowsWindow windows_window = {};
    windows::window::init(&windows_window);
    RendererState renderer_state = {};
    SimulationState simulation_state = {};
    renderer::init(&renderer_state, windows_window.handle);
    simulation::init(&simulation_state, &renderer_state, &windows_window.window);

    //RenderTarget render_texture = create_render_texture(rs, PixelFormat::R8G8B8A8_UINT_NORM);
    while(!windows_window.window.closed)
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