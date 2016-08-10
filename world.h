#pragma once
#include "object.h"
#include "dynamic_array.h"

struct World
{
    DynamicArray<Object> objects;
};

namespace world
{

void init(World* w, Allocator* allocator);

}
