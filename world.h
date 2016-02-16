#pragma once

struct World
{
    static const unsigned num_objects = 4096;
    Object objects[num_objects];
};

namespace world
{

void add_object(World* w, const Object& o);

}
