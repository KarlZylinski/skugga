#include "object.h"

namespace object
{
Object create(unsigned geometry_handle)
{
    Object o = {0};
    o.world_transform = matrix4x4::identity();
    o.geometry_handle = geometry_handle;
    return o;
}

void destroy(RendererState* rs, const Object& object)
{
    renderer::unload_geometry(rs, object.geometry_handle);
}
} // namespace object
