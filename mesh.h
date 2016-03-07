#pragma once
#include "dynamic_array.h"
#include "renderer.h"

struct Mesh
{
    DynamicArray<Vertex> vertices;
    DynamicArray<unsigned> indices;
};