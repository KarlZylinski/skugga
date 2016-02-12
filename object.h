#pragma once

#include "math.h"

struct Object
{
    bool valid;
    unsigned geometry_handle;
    Matrix4x4 world_transform;
};
