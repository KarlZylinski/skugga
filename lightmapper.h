#pragma once

struct World;
struct Renderer;

namespace lightmapper
{

void map(const World& world, Renderer* renderer);

}