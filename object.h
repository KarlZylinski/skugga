#pragma once
#include "math.h"

struct Object
{
    RRHandle geometry_handle;
    RRHandle lightmap_handle;
    unsigned id;
    Matrix4x4 world_transform;
};
