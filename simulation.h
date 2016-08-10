#include "world.h"
#include "camera.h"

struct Renderer;
struct WindowState;
struct Mesh;
struct Allocator;

struct Simulation
{
    void init(Renderer* renderer, Allocator* alloc);
    void simulate();
    Camera camera;
    World world;
};
