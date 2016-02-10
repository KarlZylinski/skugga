#pragma once

struct Object
{
    unsigned geometry_handle;
    Matrix4x4 world_transform;
};

namespace object
{
Object create(const Geometry& geometry);
void destroy(RendererState* rs, const Object& object);
}
