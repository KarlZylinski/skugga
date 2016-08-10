#pragma once

struct World;
struct Renderer;
struct Allocator;

namespace lightmapper
{

void map(World& world, Renderer* renderer);

}