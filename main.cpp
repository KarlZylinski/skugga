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
#include "radiosity_mapper.h"
#include "test_world.h"
#include "mesh.h"
#include "world.h"
#include "camera.h"

static void key_pressed_callback(Key key)
{
    keyboard_pressed(key);
}

static void key_released_callback(Key key)
{
    keyboard_released(key);
}

static void mouse_moved_callback(const Vector2i& delta)
{
    mouse_add_delta(delta);
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

    m.indices.add(0);
    m.indices.add(1);
    m.indices.add(3);
    m.indices.add(1);
    m.indices.add(2);
    m.indices.add(3);
    return m;
}

static void process_input(Camera* camera)
{
    Matrix4x4 move = matrix4x4_identity();

    if (key_is_held(Key::W))
    {
        move.w.z += 0.0005f;
    }
    if (key_is_held(Key::S))
    {
        move.w.z -= 0.0005f;
    }
    if (key_is_held(Key::A))
    {
        move.w.x -= 0.0005f;
    }
    if (key_is_held(Key::D))
    {
        move.w.x += 0.0005f;
    }

    Quaternion rotation = camera->rotation;
    Vector2i mouse_movement = mouse_movement_delta();
    if (mouse_movement.x != 0 || mouse_movement.y != 0)
    {
        rotation = quaternion_rotate_y(rotation, mouse_movement.x * 0.001f);
        rotation = quaternion_rotate_x(rotation, mouse_movement.y * 0.001f);
    }

    camera->rotation = rotation;
    Matrix4x4 camera_test_mat = matrix4x4_from_rotation_and_translation(camera->rotation, vector3_zero);
    Matrix4x4 movement_rotated = move * camera_test_mat;
    camera->position += *(Vector3*)&movement_rotated.w.x;
}


int main()
{
    void* temp_memory_block = VirtualAlloc(nullptr, TempMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    Assert(temp_memory_block != nullptr, "Failed allocating temp memory.");
    temp_memory_blob_init(temp_memory_block, TempMemorySize);

    void* permanent_memory_block = VirtualAlloc(nullptr, PermanentMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    Assert(permanent_memory_block != nullptr, "Failed allocating permanent memory.");
    permanent_memory_blob_init(permanent_memory_block, PermanentMemorySize);
    
    WindowsWindow window = {};
    create_window(&window);
    Renderer renderer = {};
    renderer.init(window.handle);

    keyboard_init();
    mouse_init();
    window.state.key_released_callback = key_released_callback;
    window.state.key_pressed_callback = key_pressed_callback;
    window.state.mouse_moved_callback = mouse_moved_callback;
    Allocator alloc = create_heap_allocator();

    bool render_fullscreen_quad = false;
    if (render_fullscreen_quad) {
        /*
        Mesh q = create_quad(&alloc);
        RRHandle tex = renderer.load_texture(pixels, PixelFormat::R32_FLOAT, 128, 128);
        alloc.dealloc(pixels);
        RRHandle quad_geo = renderer.load_geometry(q.vertices.data, q.vertices.num, q.indices.data, q.indices.num);
        mesh_destroy(&q);
        Object obj = {};
        obj.geometry_handle = quad_geo;
        obj.world_transform = matrix4x4_identity();
        obj.lightmap_handle = tex;
        World w = world_create(&alloc);
        w.objects.add(obj);

        Camera c = {};
        c.rotation = quaternion_identity();
        c.projection_matrix = matrix4x4_identity();
        c.projection_matrix.y.y = -1;

        renderer.disable_scissor();
        renderer.set_render_target(&renderer.back_buffer);
        RRHandle default_shader = renderer.load_shader("single_channel_tex.shader");
        renderer.set_shader(default_shader);

        while(!window.state.closed)
        {
            process_all_window_messsages();

            if (key_is_presssed(Key::Escape))
                window.state.closed = true;

            renderer.pre_draw_frame();
            renderer.draw_frame(w, c, DrawLights::DrawLights);
            renderer.present();
            keyboard_end_of_frame();
            mouse_end_of_frame();
        }

        world_destroy(&w);*/
    }
    else
    {
        bool run_lightmapper = true;
        if (run_lightmapper)
        {
            World mapping_world = world_create(&alloc);
            create_test_world(&mapping_world, &renderer);
            run_radiosity_mapper(mapping_world, &renderer);
            world_destroy(&mapping_world);
        }

        World world = world_create(&alloc);
        create_test_world(&world, &renderer);
        Camera camera = camera_create_projection();

        //simulation.camera.rotation = quaternion_normalize(quaternion_from_axis_angle({0,1,0}, -PI/2) * quaternion_look_at(vector3_zero,{-1,0,0}));
        //quaternion_look_at(vector3::zero, vector3::lookdir);
        renderer.disable_scissor();
        renderer.set_render_target(&renderer.back_buffer);
        RRHandle default_shader = renderer.load_shader("shader.shader");
        renderer.set_shader(default_shader);

        while(!window.state.closed)
        {
            process_all_window_messsages();
            process_input(&camera);
            renderer.pre_draw_frame();
            renderer.draw_frame(world, camera, DrawLights::DrawLights);
            renderer.present();
            keyboard_end_of_frame();
            mouse_end_of_frame();
        }

        world_destroy(&world);
    }

    renderer.shutdown();
    heap_allocator_check_clean(&alloc);

    return 0;
}