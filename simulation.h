#include "world.h"
#include "camera.h"

struct Renderer;
struct WindowState;

struct Simulation
{
    void init(Renderer* renderer, WindowState* window_state);
    void simulate();
    Camera camera;
    World world;
};
