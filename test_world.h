#pragma once
#include "object.h"

struct Renderer;
struct Mesh;
struct Vector3;
struct Color;
struct World;

namespace test_world
{

Object create_scaled_box(Renderer* renderer, const Mesh& m, const Vector3& scale, const Vector3& pos, const Color& color, unsigned id);
void create_world(World* world, Renderer* renderer);

}