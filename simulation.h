#include "world.h"
#include "camera.h"

struct SimulationState
{
    Camera camera;
    World world;
};

struct RendererState;
struct WindowState;

namespace simulation
{

void init(SimulationState* ss, RendererState* rs, WindowState* window_state);
void simulate(SimulationState* ss);

}