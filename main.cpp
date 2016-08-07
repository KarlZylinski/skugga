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
    Renderer renderer = {};
    renderer.init(window.handle);
    Simulation simulation = {};
    simulation.init(&renderer, &window.state);
    Shader lightmapping_shader = renderer.load_shader(L"uv_data.shader");
    renderer.set_shader(&lightmapping_shader);
    RenderTarget vertex_texture = renderer.create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    RenderTarget normals_texture = renderer.create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    RenderTarget* rts[] = {&vertex_texture, &normals_texture};
    renderer.set_render_targets(rts, 2);
    camera::set_lightmap_rendering_mode(&simulation.camera);

    //while(!window.state.closed)
    //{
        windows::window::process_all_messsages();
        simulation.simulate();
        renderer.draw_frame(simulation.world, simulation.camera, DrawLights::DrawLights);
        keyboard::end_of_frame();
        mouse::end_of_frame();
    //}

    Allocator ta = create_temp_allocator();
    Image vertex_image = {vertex_texture.width, vertex_texture.height, vertex_texture.pixel_format};
    image::init_data(&vertex_image, &ta);
    renderer.read_back_texture(&vertex_image, vertex_texture);
    unsigned image_size = image::size(vertex_image.pixel_format, vertex_image.width, vertex_image.height);
    Vector4* positions = (Vector4*)vertex_image.data;

    Image normals_image = {normals_texture.width, normals_texture.height, normals_texture.pixel_format};
    image::init_data(&normals_image, &ta);
    renderer.read_back_texture(&normals_image, normals_texture);
    Vector4* normals = (Vector4*)normals_image.data;

    int lax = 0;
    for (unsigned i = 0; i < image_size/16; i += 16)
    {
        if (positions[i].x != 0.0f)
        {
            lax++;

            if (lax == 40)
            {
                const Vector4& p = positions[i];
                const Vector3& n = *(Vector3*)&normals[i];
                simulation.camera.position = Vector3 {p.x, p.y, p.z};
                Vector3 forward = {0, 0, 1};
                Vector3 angle = vector3::cross(forward, n);
                float forward_len = vector3::length(forward);
                float w = sqrtf(forward_len * forward_len) + vector3::dot(forward, n);
                simulation.camera.rotation = quaternion::normalize({angle.x, angle.y, angle.z, w});
                break;
            }
        }
    }
    
    camera::set_projection_mode(&simulation.camera);
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