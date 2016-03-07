#include "world.h"

static unsigned find_free_object_handle(const World& w)
{
    for (unsigned i = 0; i < w.num_objects; ++i)
    {
        if (!w.objects[i].valid)
        {
            return i;
        }
    }

    return InvalidHandle;
}

namespace world
{

void add_object(World* w, const Object& o)
{
    unsigned object_handle = find_free_object_handle(*w);

    if (object_handle == InvalidHandle)
        return;

    w->objects[object_handle] = o;
    w->objects[object_handle].valid = true;
}

} // namespace world
