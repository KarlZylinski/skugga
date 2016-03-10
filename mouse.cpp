#include "mouse.h"

struct Mouse
{
    Vector2i delta;
};

static Mouse mouse_state;

namespace mouse
{

void init()
{
    memzero(&mouse_state, Mouse);
}

void add_delta(const Vector2i& delta)
{
    mouse_state.delta += delta;
}

const Vector2i& delta()
{
    return mouse_state.delta;
}

void end_of_frame()
{
    mouse_state.delta = {0, 0};
}

}