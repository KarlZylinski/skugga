#include "radiosity_mapper.h"
#include "renderer_direct3d.h"
#include "file.h"
#include "windows_window.h"
#include "rect.h"
#include "keyboard.h"
#include "stdio.h"

struct Patch
{
    Camera front;
    Camera right;
    Camera left;
    Camera up;
    Camera down;
    unsigned uv_index;
    ColorRGB emission;
    ColorRGB excident;
    ColorRGB incident;
    float reflectance;
};

static const unsigned LightmapSize = 64;

static const Rect scissor_full = {0, 0, LightmapSize, LightmapSize};
static const Rect scissor_top = {0, 0, LightmapSize, LightmapSize/2};
static const Rect scissor_bottom = {0, LightmapSize/2, LightmapSize, LightmapSize};
static const Rect scissor_left = {0, 0, LightmapSize/2, LightmapSize};
static const Rect scissor_right = {LightmapSize/2, 0, LightmapSize, LightmapSize};

static ColorRGB draw_hemicube_side(Renderer* renderer, const World& world, const Rect& scissor_rect,
    const Camera& camera, const RenderTarget& light_contrib_texture, Patch* patches)
{
    renderer->set_scissor_rect(scissor_rect);
    renderer->draw_frame(world, camera, DrawLights::DrawLights);
    MappedTexture m = renderer->map_texture(light_contrib_texture);

    unsigned* patch_offsets = (unsigned*)m.data;
    ColorRGB total_light = {};
    unsigned num_pixels = light_contrib_texture.width * light_contrib_texture.height;
    for (unsigned pixel_index = 0; pixel_index < num_pixels; ++pixel_index)
    {
        unsigned patch_index = patch_offsets[pixel_index];
        Patch& p = patches[patch_index];
        total_light += p.excident;
    }
    renderer->unmap_texture(m);
    return total_light;
}

void run_radiosity_mapper(World& world, Renderer* renderer)
{
    RRHandle vertex_data_shader = renderer->load_shader("uv_data.shader");
    RRHandle light_contribution_shader = renderer->load_shader("light_contribution_calc.shader");
    RenderTarget vertex_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT, LightmapSize, LightmapSize);
    RenderTarget normals_texture = renderer->create_render_texture(PixelFormat::R32G32B32A32_FLOAT, LightmapSize, LightmapSize);
    RenderTarget* vertex_data_rts[] = {&vertex_texture, &normals_texture};
    RenderTarget light_contrib_texture = renderer->create_render_texture(PixelFormat::R32_UINT, LightmapSize, LightmapSize);
    
    Allocator ta = create_temp_allocator();
    Image light_contrib_image = image_from_render_target(light_contrib_texture);
    image_init_data(&light_contrib_image, &ta);

    unsigned num_pixels = LightmapSize * LightmapSize;
    DynamicArray<Patch> patches = {&ta};
    unsigned pbo_size = sizeof(DynamicArray<unsigned>) * world.objects.num;
    DynamicArray<unsigned>* patches_by_objects = (DynamicArray<unsigned>*)ta.alloc(pbo_size);
    memset(patches_by_objects, 0, pbo_size);
    unsigned patch_offsets_size = num_pixels * sizeof(unsigned);
    unsigned* patch_offsets = (unsigned*)ta.alloc(patch_offsets_size);
    
    for (unsigned i = 0; i < world.objects.num; ++i)
    {
        Object& obj = world.objects[i];
        renderer->disable_scissor();
        renderer->set_render_targets(vertex_data_rts, 2);
        renderer->set_shader(vertex_data_shader);
        renderer->pre_draw_frame();
        Camera vertex_data_camera = camera_create_uv_rendering();
        renderer->draw(obj, camera_calc_view_matrix(vertex_data_camera), vertex_data_camera.projection_matrix);
        renderer->present();

        MappedTexture vertex_image = renderer->map_texture(vertex_texture);
        Vector4* positions = (Vector4*)vertex_image.data;
        MappedTexture normals_image = renderer->map_texture(normals_texture);
        Vector4* normals = (Vector4*)normals_image.data;

        Patch base_patch = {};
        if (obj.is_light)
        {
            base_patch.emission.r = base_patch.emission.g = base_patch.emission.b = 1.0f;
            base_patch.excident = base_patch.emission;
        }

        memset(patch_offsets, 0, patch_offsets_size);

        for (unsigned pixel_index = 0; pixel_index < num_pixels; ++pixel_index)
        {
            const Vector3& n = *(Vector3*)&normals[pixel_index];

            if (n.x != 0.0f || n.y != 0.0f || n.z != 0.0f)
            {
                const Vector3& pos = *(Vector3*)&positions[pixel_index];

                Camera base_cam = camera_create_projection();
                base_cam.position = {pos.x, pos.y, pos.z};

                Patch p = base_patch;
                p.front = base_cam;
                static const Vector3 origin = {0,0,0};
                p.front.rotation = quaternion_look_at(origin, n);
                Vector3 bitangent = vector3_bitangent(n);
                Vector3 tangent = vector3_tangent(n);

                p.right = base_cam;
                p.right.rotation = quaternion_normalize(quaternion_from_axis_angle(bitangent, PI/2) * p.front.rotation);

                p.left = base_cam;
                p.left.rotation = quaternion_normalize(quaternion_from_axis_angle(bitangent, -PI/2) * p.front.rotation);

                p.up = base_cam;
                p.up.rotation = quaternion_normalize(quaternion_from_axis_angle(tangent, PI/2) * p.front.rotation);

                p.down = base_cam;
                p.down.rotation = quaternion_normalize(quaternion_from_axis_angle(tangent, -PI/2) * p.front.rotation);

                p.reflectance = 0.5f;
                p.uv_index = pixel_index;
                patch_offsets[pixel_index] = patches.num;
                patches.add(p);

                DynamicArray<unsigned>& pbo = patches_by_objects[i];

                if (pbo.allocator == nullptr)
                    pbo.allocator = &ta;

                pbo.add(patches.num - 1);
            }
        }

        renderer->unmap_texture(vertex_image);
        renderer->unmap_texture(normals_image);

        RRHandle tex_handle = renderer->load_texture(patch_offsets, PixelFormat::R32_UINT, LightmapSize, LightmapSize);
        Assert(IsValidRRHandle(tex_handle), "Failed uploading offsets texture to GPU in lightmapper.");
        obj.lightmap_patch_offset = tex_handle;
    }

    renderer->set_shader(light_contribution_shader);
    renderer->set_render_target(&light_contrib_texture);

    for (unsigned pass = 0; pass < 1; ++pass)
    {
        for (unsigned patch_index = 0; patch_index < patches.num; ++patch_index)
        {
            Patch& p = patches[patch_index];
            process_all_window_messsages();

            if (key_is_presssed(Key::Escape))
            {
                return;
            }

            ColorRGB incident = {};
            incident += draw_hemicube_side(renderer, world, scissor_full, p.front, light_contrib_texture, patches.data);
            incident += draw_hemicube_side(renderer, world, scissor_left, p.right, light_contrib_texture, patches.data);
            incident += draw_hemicube_side(renderer, world, scissor_right, p.left, light_contrib_texture, patches.data);
            incident += draw_hemicube_side(renderer, world, scissor_bottom, p.up, light_contrib_texture, patches.data);
            incident += draw_hemicube_side(renderer, world, scissor_top, p.down, light_contrib_texture, patches.data);
            p.incident.r = min(incident.r, 1);
            p.incident.g = min(incident.g, 1);
            p.incident.b = min(incident.b, 1);
        }

        for (unsigned patch_index = 0; patch_index < patches.num; ++patch_index)
        {
            Patch& p = patches[patch_index];
            p.excident = p.incident * p.reflectance + p.emission;
        }
    }

    Image lightmap = {};
    lightmap.width = LightmapSize;
    lightmap.height = LightmapSize;
    lightmap.pixel_format = PixelFormat::R8G8B8A8_UINT_NORM;
    image_init_data(&lightmap, &ta);
    unsigned lightmap_size = image_size(lightmap);
    for (unsigned obj_index = 0; obj_index < world.objects.num; ++obj_index)
    {
        memset(lightmap.data, 0, lightmap_size);
        DynamicArray<unsigned>& pbo = patches_by_objects[obj_index];

        for (unsigned pi = 0; pi < pbo.num; ++pi)
        {
            const Patch& p = patches[pbo[pi]];
            ColorUNorm& out_color = ((ColorUNorm*)lightmap.data)[p.uv_index];
            out_color.r = ((unsigned char)min(p.incident.r, 1.0f)) * 255;
            out_color.g = ((unsigned char)min(p.incident.g, 1.0f)) * 255;
            out_color.b = ((unsigned char)min(p.incident.b, 1.0f)) * 255;
            out_color.a = 255;
        }

        char filename[10];
        sprintf(filename, "%d.data", world.objects[obj_index].id);
        file_write(lightmap.data, lightmap_size, filename);
    }
}
