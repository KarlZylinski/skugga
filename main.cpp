#include <windows.h>
#include <time.h>
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
    Shader lightmapping_shader = renderer::load_shader(&renderer_state, L"uv_data.shader");
    renderer::set_shader(&renderer_state, &lightmapping_shader);
    RenderTarget render_texture = renderer::create_render_texture(&renderer_state, PixelFormat::R32G32B32A32_FLOAT);
    renderer::set_render_target(&renderer_state, &render_texture);
    camera::set_lightmap_rendering_mode(&simulation_state.camera);

    //while(!window.state.closed)
    //{
        windows::window::process_all_messsages();
        simulation::simulate(&simulation_state);
        renderer::draw_frame(&renderer_state, simulation_state.world, simulation_state.camera);
        keyboard::end_of_frame();
        mouse::end_of_frame();
    //}

    Allocator ta = create_temp_allocator();
    Image i = renderer::read_back_texture(&ta, &renderer_state, render_texture);
    unsigned image_size = image::size(i.pixel_format, i.width, i.height);
    Vector4* positions = (Vector4*)i.data;

    int lax = 0;
    for (unsigned i = 0; i < image_size/16; i += 16)
    {
        if (positions[i].x != 0.0f)
        {
            lax++;

            if (lax == 40)
            {
                const Vector4& p = positions[i];
                simulation_state.camera.position = Vector3 {p.x, p.y, p.z};
                break;
            }
        }
    }
    
    renderer::set_render_target(&renderer_state, &renderer_state.back_buffer);
    Shader default_shader = renderer::load_shader(&renderer_state, L"shader.shader");
    renderer::set_shader(&renderer_state, &default_shader);
    camera::set_projection_mode(&simulation_state.camera);
    //simulation_state.camera.position = Vector3 { camera_pos.x, camera_pos.y, camera_pos.z };

    while(!window.state.closed)
    {
        windows::window::process_all_messsages();
        simulation::simulate(&simulation_state);
        renderer::draw_frame(&renderer_state, simulation_state.world, simulation_state.camera);
        keyboard::end_of_frame();
        mouse::end_of_frame();
    }

    renderer::shutdown(&renderer_state);
    return 0;
}