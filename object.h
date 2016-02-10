#pragma once

struct Object
{
    unsigned geometry_handle;
    Matrix4x4 world_transform;
};

namespace object
{
Object create(RendererState* rs, const Geometry& geometry);
}
