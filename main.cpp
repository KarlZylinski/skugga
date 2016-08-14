#include <windows.h>
#include <stdio.h>
#include "object.h"
#include "types.h"
#include "helpers.h"
#include "memory.h"
#include "windows_window.h"
#include "renderer_direct3d.h"
#include "keyboard.h"
#include "mouse.h"
#include "file.h"
#include "lightmapper.h"
#include "test_world.h"
#include "mesh.h"

static void key_pressed_callback(Key key)
{
    keyboard::pressed(key);
}

static void key_released_callback(Key key)
{
    keyboard::released(key);
}

static void mouse_moved_callback(const Vector2i& delta)
{
    mouse::add_delta(delta);
}

static void create_distortion_compensation_texture(unsigned char* pixels, unsigned size)
{
    const Vector3 observation_dir = {0,0,1};
    const Vector3 observation_pos = {float(size/2),float(size/2),0};
    const float plane_z = (float)(size/2);
    const unsigned num_pixels = size*size;
    for (unsigned i = 0; i < num_pixels; ++i)
    {
        float x = (float)(i % size);
        float y = (float)(i / size);
        Vector3 p = {x, y, plane_z};
        pixels[i] = (unsigned char)(vector3::dot(observation_dir, vector3::normalize(p - observation_pos)) * 255);
    }
}

static void create_cosine_law_texture(unsigned char* pixels, unsigned size)
{
    const float fourth_size = (float)(size/4);
    const float half_size = (float)(size/2);
    const float double_size = (float)(size*2);
    const unsigned num_pixels = size*size;
    const Vector3 normal = {0, 1, 0};
    const Vector3 pos = {0, 0, 0};
    for (unsigned i = 0; i < num_pixels; ++i)
    {
        const float x = (float)(i % size) - half_size;
        const float z = (float)(i / size) - half_size;
        const float hx = x/2;
        const float hz = z/2;
        const float y1 = sqrtf(fourth_size * fourth_size - hx*hx - hz*hz);
        const Vector3 light_pos = {hx, y1, hz};
        const unsigned char color = (unsigned char)(vector3::dot(normal, vector3::normalize(light_pos - pos)) * 255);
        pixels[i] = color;
    }
}

static Mesh create_quad(Allocator* alloc)
{
    Mesh m = mesh_create(alloc);
    Vertex b = {};

    Vertex v1 = b;
    v1.position = {-1,-1,0};
    v1.uv = {0,0};
    m.vertices.add(v1);

    Vertex v2 = b;
    v2.position = {1,-1,0};
    v2.uv = {1,0};
    m.vertices.add(v2);

    Vertex v3 = b;
    v3.position = {1,1,0};
    v3.uv = {1,1};
    m.vertices.add(v3);

    Vertex v4 = b;
    v4.position = {-1,1,0};
    v4.uv = {0,1};
    m.vertices.add(v4);

    m.indices.add(1);
    m.indices.add(0);
    m.indices.add(3);
    m.indices.add(2);
    m.indices.add(1);
    m.indices.add(3);
    return m;
}

static void process_input(Camera* camera)
{
    Matrix4x4 move = matrix4x4::identity();

    if (keyboard::is_held(Key::W))
    {
        move.w.z += 0.0005f;
    }
    if (keyboard::is_held(Key::S))
    {
        move.w.z -= 0.0005f;
    }
    if (keyboard::is_held(Key::A))
    {
        move.w.x -= 0.0005f;
    }
    if (keyboard::is_held(Key::D))
    {
        move.w.x += 0.0005f;
    }

    Quaternion rotation = camera->rotation;

    if (mouse::delta().x != 0 || mouse::delta().y != 0)
    {
        rotation = quaternion::rotate_y(rotation, mouse::delta().x * 0.001f);
        rotation = quaternion::rotate_x(rotation, mouse::delta().y * 0.001f);
    }

    camera->rotation = rotation;
    Matrix4x4 camera_test_mat = matrix4x4::from_rotation_and_translation(camera->rotation, {0,0,0});
    Matrix4x4 movement_rotated = move * camera_test_mat;
    camera->position += *(Vector3*)&movement_rotated.w.x;
}


int main()
{

    void* temp_memory_block = VirtualAlloc(nullptr, temp_memory::TempMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    temp_memory::init(temp_memory_block, temp_memory::TempMemorySize);
    
    windows::Window window = {};
    windows::window::init(&window);
    Renderer renderer = {};
    renderer.init(window.handle);

    keyboard::init();
    mouse::init();
    window.state.key_released_callback = key_released_callback;
    window.state.key_pressed_callback = key_pressed_callback;
    window.state.mouse_moved_callback = mouse_moved_callback;
    Allocator alloc = create_debug_allocator();

    bool render_fullscreen_quad = false;
    if (render_fullscreen_quad) {
        unsigned s = 128;
        unsigned num_pixels = 128*128;
        unsigned size = num_pixels*sizeof(unsigned char);
        unsigned char* pixels = (unsigned char*)malloc(size);
        memset(pixels, 0, size);
        create_cosine_law_texture(pixels, s);
        Mesh q = create_quad(&alloc);
        RRHandle tex = renderer.load_texture(pixels, PixelFormat::R8_UINT_NORM, 128, 128);
        RRHandle quad_geo = renderer.load_geometry(q.vertices.data, q.vertices.num, q.indices.data, q.indices.num);
        Object obj = {};
        obj.geometry_handle = quad_geo;
        obj.world_transform = matrix4x4::identity();
        obj.lightmap_handle = tex;
        World w = world_create(&alloc);
        w.objects.add(obj);

        Camera c = {};
        c.rotation = quaternion::identity();
        c.projection_matrix = matrix4x4::identity();

        renderer.disable_scissor();
        renderer.set_render_target(&renderer.back_buffer);
        RRHandle default_shader = renderer.load_shader(L"single_channel_tex.shader");
        renderer.set_shader(default_shader);

        while(!window.state.closed)
        {
            windows::window::process_all_messsages();
            renderer.pre_draw_frame();
            renderer.draw_frame(w, c, DrawLights::DrawLights);
            renderer.present();
            keyboard::end_of_frame();
            mouse::end_of_frame();
        }
    }
    else
    {
        bool run_lightmapper = false;
        if (run_lightmapper)
        {
            World mapping_world = world_create(&alloc);
            test_world::create_world(&mapping_world, &renderer);
            lightmapper::map(mapping_world, &renderer);
        }

        World world = world_create(&alloc);
        test_world::create_world(&world, &renderer);
        Camera camera = camera_create_projection();

        //simulation.camera.rotation = quaternion::normalize(quaternion::from_axis_angle({0,1,0}, -PI/2) * quaternion::look_at({0,0,0},{-1,0,0}));
        //quaternion::look_at(vector3::zero, vector3::lookdir);
        renderer.disable_scissor();
        renderer.set_render_target(&renderer.back_buffer);
        RRHandle default_shader = renderer.load_shader(L"shader.shader");
        renderer.set_shader(default_shader);

        while(!window.state.closed)
        {
            windows::window::process_all_messsages();
            process_input(&camera);
            renderer.pre_draw_frame();
            renderer.draw_frame(world, camera, DrawLights::DrawLights);
            renderer.present();
            keyboard::end_of_frame();
            mouse::end_of_frame();
        }
    }

    renderer.shutdown();

    return 0;
}