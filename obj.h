#pragma once
#include "mesh.h"

struct Allocator;

struct LoadedMesh
{
    bool valid;
    Mesh mesh;
};

namespace obj
{

LoadedMesh load(Allocator* alloc, const wchar* filename);

}