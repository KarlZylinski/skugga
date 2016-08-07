#include "lightmapper.h"
#include "renderer_direct3d.h"
#include "file.h"
#include "windows_window.h"

namespace lightmapper
{

void map(const World& world, Renderer* renderer)
{
    Shader vertex_data_shader = renderer->load_shader(L"uv_data.shader");
    Shader light_contribution_shader = renderer->load_shader(L"light_contribution_calc.shader");
    RenderTarget vertex_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    RenderTarget normals_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    RenderTarget* vertex_data_rts[] = {&vertex_texture, &normals_texture};
    RenderTarget light_contrib_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    Camera vertex_data_camera;
    camera::set_lightmap_rendering_mode(&vertex_data_camera);
    Matrix4x4 view_matrix = camera::calc_view_matrix(vertex_data_camera);

    unsigned num_lights = 0;
    const Object* lights[world.num_lights];
    for (unsigned i = 0; i < world.num_lights; ++i)
    {
        if (world.lights[i].valid)
        {
            lights[num_lights] = &world.lights[i];
            ++num_lights;
        }
    }

    Allocator ta = create_temp_allocator();
    Image vertex_image = {vertex_texture.width, vertex_texture.height, vertex_texture.pixel_format};
    image::init_data(&vertex_image, &ta);
    Image normals_image = {normals_texture.width, normals_texture.height, normals_texture.pixel_format};
    image::init_data(&normals_image, &ta);
    Image light_contrib_image = {light_contrib_texture.width, light_contrib_texture.height, light_contrib_texture.pixel_format};
    image::init_data(&light_contrib_image, &ta);

    Image lightmap = {light_contrib_texture.width, light_contrib_texture.height, PixelFormat::R8G8B8A8_UINT_NORM};
    image::init_data(&lightmap, &ta);
    unsigned lightmap_size = image::size(lightmap);

    for (unsigned i = 0; i < world.num_objects; ++i)
    {
        if (!world.objects[i].valid)
            continue;

        renderer->set_render_targets(vertex_data_rts, 2);
        renderer->set_shader(&vertex_data_shader);
        renderer->clear_depth_stencil();
        renderer->clear_render_target(&vertex_texture, {0, 0, 0, 1});
        renderer->clear_render_target(&normals_texture, {0, 0, 0, 1});
        renderer->draw(world.objects[i].geometry_handle, world.objects[i].world_transform, view_matrix, vertex_data_camera.projection_matrix, lights, num_lights);
        renderer->present();

        renderer->read_back_texture(&vertex_image, vertex_texture);
        unsigned image_size = image::size(vertex_image.pixel_format, vertex_image.width, vertex_image.height);
        Vector4* positions = (Vector4*)vertex_image.data;

        renderer->read_back_texture(&normals_image, normals_texture);
        Vector4* normals = (Vector4*)normals_image.data;

        memset(lightmap.data, 0, lightmap_size);

        for (unsigned pixel_index = 0; pixel_index < image_size/16; ++pixel_index)
        {
            const Vector3& n = *(Vector3*)&normals[pixel_index];

            if (n.x == 0.0f && n.y == 0.0f && n.z == 0.0f)
                continue;

            const Vector4& p = positions[pixel_index];
            static Camera patch_camera;
            camera::set_projection_mode(&patch_camera);
            patch_camera.position = Vector3 {p.x, p.y, p.z};
            
            static const Vector3 forward = {0, 0, 1};
            const Vector3 angle = vector3::cross(forward, n);
            float forward_len = vector3::length(forward);
            float w = sqrtf(forward_len * forward_len) + vector3::dot(forward, n);
            patch_camera.rotation = quaternion::normalize({angle.x, angle.y, angle.z, w});

            windows::window::process_all_messsages();
            renderer->clear_render_target(&light_contrib_texture, {0, 0, 0, 1});
            renderer->set_render_target(&light_contrib_texture);
            renderer->set_shader(&light_contribution_shader);
            renderer->draw_frame(world, patch_camera, DrawLights::DrawLights);

            renderer->read_back_texture(&light_contrib_image, light_contrib_texture);

            Color* contrib_pixels = (Color*)light_contrib_image.data;
            ColorUNorm& current = *(ColorUNorm*)&lightmap.data[pixel_index * 4];
            current.a = 255;
            for (unsigned contrib_index = 0; contrib_index < image_size/16; ++contrib_index)
            {
                const Color& contrib_pixel = contrib_pixels[contrib_index];
                current.r = (unsigned char)min(255.0f, (contrib_pixel.r*255.0f) + current.r);
                current.g = (unsigned char)min(255.0f, (contrib_pixel.g*255.0f) + current.g);
                current.b = (unsigned char)min(255.0f, (contrib_pixel.b*255.0f) + current.b);
            }
        }

        File lightmap_file;
        lightmap_file.data = lightmap.data;
        lightmap_file.size = lightmap_size;
        wchar filename[10];
        wsprintf(filename, L"%d.data", i);
        file::write(lightmap_file, filename);
    }
}

}