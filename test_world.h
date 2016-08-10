#pragma once
#include "object.h"

struct Renderer;
struct Mesh;
struct Vector3;
struct Color;
struct World;

namespace test_world
{

void create_world(World* world, Renderer* renderer);

}