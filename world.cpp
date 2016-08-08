#include "world.h"

static unsigned find_free_object_handle(Object* objects, unsigned num_objects)
{
    for (unsigned i = 1; i < num_objects; ++i)
    {
        if (!objects[i].valid)
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
    unsigned object_handle = find_free_object_handle(w->objects, w->num_objects);

    if (object_handle == InvalidHandle)
        return;

    w->objects[object_handle] = o;
    w->objects[object_handle].valid = true;
}

void add_light(World* w, const Object& o)
{
    unsigned object_handle = find_free_object_handle(w->lights, w->num_lights);

    if (object_handle == InvalidHandle)
        return;

    w->lights[object_handle] = o;
    w->lights[object_handle].valid = true;
}

} // namespace world
