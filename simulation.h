#include "world.h"
#include "camera.h"

struct Renderer;
struct WindowState;
struct Mesh;
struct Allocator;

struct Simulation
{
    void init(Renderer* renderer, WindowState* window_state, Allocator* alloc);
    void simulate();
    Camera camera;
    World world;
};
