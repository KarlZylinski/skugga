#pragma once
#include "object.h"

struct World
{
    static const unsigned num_objects = 4096;
    static const unsigned num_lights = 4096;
    Object objects[num_objects];
    Object lights[num_lights];
};

namespace world
{

void add_object(World* w, const Object& o);
void add_light(World* w, const Object& o);

}
