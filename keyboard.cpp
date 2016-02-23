#include "keyboard.h"

struct Keyboard
{
    bool held[Key::NumKeys];
    bool pressed[Key::NumKeys];
    bool released[Key::NumKeys];
};

Keyboard keyboard_state;

namespace keyboard
{

void init()
{
    memzero(&keyboard_state, Keyboard);
}

bool held(Key key)
{
    return keyboard_state.held[(unsigned)key];
}

bool presssed(Key key)
{
    return keyboard_state.pressed[(unsigned)key];
}

bool released(Key key)
{
    return keyboard_state.released[(unsigned)key];
}

void end_of_frame()
{
    memset(keyboard_state.pressed, 0, sizeof(Key) * (unsigned)Key::NumKeys);
    memset(keyboard_state.released, 0, sizeof(Key) * (unsigned)Key::NumKeys);
}

}
