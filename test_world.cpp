#include <windows.h>
#include "test_world.h"
#include "object.h"
#include "world.h"
#include "renderer_direct3d.h"
#include "mesh.h"
#include "obj.h"

namespace test_world
{

Object create_scaled_box(Renderer* renderer, const Mesh& m, const Vector3& scale, const Vector3& pos, const Color& color, unsigned id)
{
    Vertex* scaled_vertices = m.vertices.clone_raw();
    memcpy(scaled_vertices, m.vertices.data, m.vertices.num * sizeof(Vertex));

    for (unsigned i = 0; i < m.vertices.num; ++i)
    {
        scaled_vertices[i].position = scaled_vertices[i].position * scale;
        scaled_vertices[i].color = color;
    }

    RRHandle box_geometry_handle = renderer->load_geometry(scaled_vertices, m.vertices.num, m.indices.data, m.indices.num);
    Object obj = {0};
    obj.geometry_handle = box_geometry_handle;
    obj.world_transform = matrix4x4::identity();
    obj.id = id;
    memcpy(&obj.world_transform.w.x, &pos.x, sizeof(Vector3));

    static wchar lightmap_filename[256];
    wsprintf(lightmap_filename, L"%d.data", id);
    Allocator ta = create_temp_allocator();
    RRHandle lightmap_handle = renderer->load_texture(&ta, lightmap_filename);

    if (IsValidRRHandle(lightmap_handle))
    {
        obj.lightmap_handle = lightmap_handle;
    }

    return obj;
}

void create_world(World* world, Renderer* renderer)
{
    Allocator ta = create_temp_allocator();
    LoadedMesh lm = obj::load(&ta, L"box.wobj");

    if (!lm.valid)
        return;

    float floor_width = 6;
    float floor_depth = 8;
    float floor_thickness = 0.3f;
    float floor_to_cieling = 2;
    float pillar_width = 0.4f;

    world->objects.add(create_scaled_box(renderer, lm.mesh, {floor_width, floor_thickness, floor_depth}, {0, 0, 0}, color::random(), 4));
    world->objects.add(create_scaled_box(renderer, lm.mesh, {pillar_width, floor_to_cieling, pillar_width}, {-1, (floor_thickness + floor_to_cieling) / 2, 1}, color::random(), 12));
    world->objects.add(create_scaled_box(renderer, lm.mesh, {pillar_width, floor_to_cieling, pillar_width}, {-1, (floor_thickness + floor_to_cieling) / 2, -1}, color::random(), 123));
    world->objects.add(create_scaled_box(renderer, lm.mesh, {floor_width, floor_thickness, floor_depth}, {0, floor_thickness + floor_to_cieling, 0}, color::random(), 145));
    //world->objects.add(create_scaled_box(renderer, lm.mesh, {floor_width, floor_thickness, floor_depth}, {0, floor_thickness + floor_to_cieling - 15, 0}, color::random(), 12333));

    //world->objects.add(create_scaled_box(renderer, lm.mesh, {2,2,2}, {10, -20, 0}, color::random(), 145));

    //world->objects.add(create_scaled_box(renderer, lm.mesh, {1,1,1}, vector3::lookdir * 5, color::random(), 145));

    //world::add_object(world, create_scaled_box(renderer, lm.mesh, {1, 1, 1}, {-10, 0, 0}, color::random()));

    if (lm.valid)
    {
        for (unsigned i = 0; i < lm.mesh.vertices.num; ++i)
        {
            lm.mesh.vertices[i].light_emittance = 1.0f;
        }

        world->lights.add(create_scaled_box(renderer, lm.mesh, {10, 10, 10}, {20, 25, -19}, {1,1,1,1}, 10000));
    }
}

}
