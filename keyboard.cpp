#include "keyboard.h"

struct Keyboard
{
    bool held[Key::NumKeys];
    bool pressed[Key::NumKeys];
    bool released[Key::NumKeys];
};

static Keyboard keyboard_state;

namespace keyboard
{

void init()
{
    memzero(&keyboard_state, Keyboard);
}

bool is_held(Key key)
{
    return keyboard_state.held[(unsigned)key];
}

bool is_presssed(Key key)
{
    return keyboard_state.pressed[(unsigned)key];
}

bool is_released(Key key)
{
    return keyboard_state.released[(unsigned)key];
}

void pressed(Key key)
{
    keyboard_state.pressed[(unsigned)key] = true;
    keyboard_state.held[(unsigned)key] = true;
}

void released(Key key)
{
    keyboard_state.released[(unsigned)key] = true;
    keyboard_state.held[(unsigned)key] = false;
}

void end_of_frame()
{
    memset(keyboard_state.pressed, 0, sizeof(bool) * (unsigned)Key::NumKeys);
    memset(keyboard_state.released, 0, sizeof(bool) * (unsigned)Key::NumKeys);
}

}
