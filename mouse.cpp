#include "mouse.h"

struct Mouse
{
    Vector2i delta;
};

Mouse mouse_state;

namespace mouse
{

void init()
{
    memzero(&mouse_state, Mouse);
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