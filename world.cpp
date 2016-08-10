#include "world.h"

namespace world
{

void init(World* w, Allocator* allocator)
{
    w->objects.allocator = allocator;
}

}
