#include "lightmapper.h"
#include "renderer_direct3d.h"

namespace lightmapper
{

void map(const World& world, Renderer* renderer)
{
    Shader lightmapping_shader = renderer->load_shader(L"uv_data.shader");
    renderer->set_shader(&lightmapping_shader);
    RenderTarget vertex_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    RenderTarget normals_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    RenderTarget* rts[] = {&vertex_texture, &normals_texture};
    renderer->set_render_targets(rts, 2);
    Camera camera;
    camera::set_lightmap_rendering_mode(&camera);
    renderer->draw_frame(world, camera, DrawLights::DrawLights);

    Allocator ta = create_temp_allocator();
    Image vertex_image = {vertex_texture.width, vertex_texture.height, vertex_texture.pixel_format};
    image::init_data(&vertex_image, &ta);
    renderer->read_back_texture(&vertex_image, vertex_texture);
    unsigned image_size = image::size(vertex_image.pixel_format, vertex_image.width, vertex_image.height);
    Vector4* positions = (Vector4*)vertex_image.data;

    Image normals_image = {normals_texture.width, normals_texture.height, normals_texture.pixel_format};
    image::init_data(&normals_image, &ta);
    renderer->read_back_texture(&normals_image, normals_texture);
    Vector4* normals = (Vector4*)normals_image.data;
    (void)normals;(void)positions;(void)image_size;
    
    /*int lax = 0;
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
    }*/
}

}