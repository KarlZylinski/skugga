#include "object.h"

namespace object
{
Object create(RendererState* rs, unsigned geometry_handle)
{
    Object o = {0};
    o.world_transform = matrix4x4::identity();
    o.geometry_handle = geometry_handle;
    return o;
}
} // namespace object
