#include "world.h"
#include "camera.h"

struct Renderer;
struct WindowState;
struct Mesh;

struct Simulation
{
    void init(Renderer* renderer, WindowState* window_state);
    void simulate();
    void create_light(Renderer* renderer, Mesh* mesh, const Vector3& position);
    Camera camera;
    World world;
    Object* sun;
};
