#pragma once

#include "math.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

struct Object
{
    bool valid;
    RRHandle geometry_handle;
    RRHandle lightmap_handle;
    unsigned id;
    Matrix4x4 world_transform;
};
