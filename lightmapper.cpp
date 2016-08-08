#include "lightmapper.h"
#include "renderer_direct3d.h"
#include "file.h"
#include "windows_window.h"
#include "rect.h"
#include "config.h"
#include "keyboard.h"

namespace lightmapper
{

struct Patch
{
    Camera front;
    Camera right;
    Camera left;
    Camera up;
    Camera down;
    unsigned pixel_index;
};

static const Rect scissor_full = {0, 0, WindowWidth, WindowHeight};
static const Rect scissor_top = {0, 0, WindowWidth, WindowHeight/2};
static const Rect scissor_bottom = {0, WindowHeight/2, WindowWidth, WindowHeight};
static const Rect scissor_left = {0, 0, WindowWidth/2, WindowHeight};
static const Rect scissor_right = {WindowWidth/2, 0, WindowWidth, WindowHeight};

Color draw_hemicube_side(Renderer* renderer, const World& world, const Rect& scissor_rect,
    const Camera& camera, const RenderTarget& light_contrib_texture, Image* light_contrib_image)
{
    renderer->set_scissor_rect(scissor_rect);
    renderer->draw_frame(world, camera, DrawLights::DrawLights);
    renderer->read_back_texture(light_contrib_image, light_contrib_texture);

    Color* contrib_pixels = (Color*)light_contrib_image->data;
    Color total_light = {};
    unsigned num_pixels = light_contrib_texture.width * light_contrib_texture.height;
    for (unsigned contrib_index = 0; contrib_index < num_pixels; ++contrib_index)
    {
        const Color& cp = contrib_pixels[contrib_index];
        total_light.r += cp.r;
        total_light.g += cp.g;
        total_light.b += cp.b;
    }
    return total_light;
}

void map(const World& world, Renderer* renderer)
{
    RRHandle vertex_data_shader = renderer->load_shader(L"uv_data.shader");
    RRHandle light_contribution_shader = renderer->load_shader(L"light_contribution_calc.shader");
    RenderTarget vertex_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    RenderTarget normals_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    RenderTarget* vertex_data_rts[] = {&vertex_texture, &normals_texture};
    RenderTarget light_contrib_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT);
    Camera vertex_data_camera;
    camera::set_lightmap_rendering_mode(&vertex_data_camera);
    Matrix4x4 view_matrix = camera::calc_view_matrix(vertex_data_camera);

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

    unsigned num_pixels = vertex_image.width * vertex_image.height;
    Patch* patches = (Patch*)ta.alloc(num_pixels * sizeof(Patch));

    for (unsigned i = 0; i < world.num_objects; ++i)
    {
        if (!world.objects[i].valid)
            continue;

        renderer->set_render_targets(vertex_data_rts, 2);
        renderer->set_shader(vertex_data_shader);
        renderer->pre_draw_frame();
        renderer->draw(world.objects[i], view_matrix, vertex_data_camera.projection_matrix);
        renderer->present();

        renderer->read_back_texture(&vertex_image, vertex_texture);
        Vector4* positions = (Vector4*)vertex_image.data;
        renderer->read_back_texture(&normals_image, normals_texture);
        Vector4* normals = (Vector4*)normals_image.data;

        unsigned num_patches = 0;
        for (unsigned pixel_index = 0; pixel_index < num_pixels; ++pixel_index)
        {
            const Vector3& n = *(Vector3*)&normals[pixel_index];

            if (n.x == 0.0f && n.y == 0.0f && n.z == 0.0f)
                continue;

            Patch& p = patches[num_patches];
            ++num_patches;
            const Vector3& pos = *(Vector3*)&positions[pixel_index];

            Camera base_cam = {};
            camera::set_projection_mode(&base_cam);
            base_cam.position = {pos.x, pos.y, pos.z};

            p.front = base_cam;
            static Vector3 origin = {0,0,0};
            p.front.rotation = quaternion::look_at(origin, n);
            Vector3 bitangent = vector3::bitangent(n);
            Vector3 tangent = vector3::tangent(n);

            p.right = base_cam;
            p.right.rotation = quaternion::normalize(quaternion::from_axis_angle(bitangent, PI/2) * p.front.rotation);

            p.left = base_cam;
            p.left.rotation = quaternion::normalize(quaternion::from_axis_angle(bitangent, -PI/2) * p.front.rotation);

            p.up = base_cam;
            p.up.rotation = quaternion::normalize(quaternion::from_axis_angle(tangent, PI/2) * p.front.rotation);

            p.down = base_cam;
            p.down.rotation = quaternion::normalize(quaternion::from_axis_angle(tangent, -PI/2) * p.front.rotation);

            p.pixel_index = pixel_index;
        }

        memset(lightmap.data, 0, lightmap_size);
        renderer->set_shader(light_contribution_shader);
        renderer->set_render_target(&light_contrib_texture);

        for (unsigned patch_index = 0; patch_index < num_patches; ++patch_index)
        {
            const Patch& p = patches[patch_index];
            windows::window::process_all_messsages();

            if (keyboard::is_presssed(Key::Escape))
            {
                return;
            }

            Color c = {};
            c += draw_hemicube_side(renderer, world, scissor_full, p.front, light_contrib_texture, &light_contrib_image);
            c += draw_hemicube_side(renderer, world, scissor_left, p.right, light_contrib_texture, &light_contrib_image);
            c += draw_hemicube_side(renderer, world, scissor_right, p.left, light_contrib_texture, &light_contrib_image);
            c += draw_hemicube_side(renderer, world, scissor_bottom, p.up, light_contrib_texture, &light_contrib_image);
            c += draw_hemicube_side(renderer, world, scissor_top, p.down, light_contrib_texture, &light_contrib_image);
            ColorUNorm& out_color = ((ColorUNorm*)lightmap.data)[p.pixel_index];
            out_color.r = ((unsigned char)min(c.r, 1.0f)) * 255;
            out_color.g = ((unsigned char)min(c.g, 1.0f)) * 255;
            out_color.b = ((unsigned char)min(c.b, 1.0f)) * 255;
            out_color.a = 255;
        }

        File lightmap_file;
        lightmap_file.data = lightmap.data;
        lightmap_file.size = lightmap_size;
        wchar filename[10];
        wsprintf(filename, L"%d.data", world.objects[i].id);
        file::write(lightmap_file, filename);
    }
}

}