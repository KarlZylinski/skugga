#pragma once

struct World;
struct Renderer;
struct Allocator;

void run_radiosity_mapper(World& world, Renderer* renderer);
