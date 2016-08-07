#pragma once

#include "math.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

struct Object
{
    bool valid;
    unsigned geometry_handle;
    unsigned id;
    Matrix4x4 world_transform;
    ID3D11Texture2D* lightmap;
    ID3D11ShaderResourceView* lightmap_resource_view;
};
